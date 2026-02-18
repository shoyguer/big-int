"""
upgrade_tool.py
----------------
Script to upgrade a godot-plus-plus plugin project to target
Godot 4.6 (godot-cpp master branch).

Usage:
    # run from the plugin root
    python upgrade_tool.py
    # or pass the root explicitly
    python upgrade_tool.py <plugin_root>

What it does:
  1. Initialises the godot-cpp submodule if it hasn't been yet.
  2. Switches the godot-cpp submodule to 'master' (= Godot 4.6 dev).
  3. Updates .gitmodules  branch = master
  4. Runs git submodule sync
  5. Updates dont_touch.txt  version line -> 4.6
  6. Updates compatibility_minimum in the .gdextension file.
     By default it is set to "4.6" so the plugin still loads on Godot 4.6.
     Pass --min 4.6 to restrict it to 4.6+ only.
  7. Cleans old build files with scons -c  (pass --no-clean to skip).
"""

import os
import re
import sys
import subprocess

# CLI args
def parse_args():
    args = sys.argv[1:]
    plugin_root = None
    compat_min = "4.6"
    no_clean = False

    positional = []
    i = 0
    while i < len(args):
        if args[i] == "--min" and i + 1 < len(args):
            compat_min = args[i + 1]
            i += 2
        elif args[i] == "--no-clean":
            no_clean = True
            i += 1
        else:
            positional.append(args[i])
            i += 1

    if positional:
        plugin_root = os.path.abspath(positional[0])
    else:
        # default: directory that contains this script's parent, or cwd
        script_dir = os.path.dirname(os.path.abspath(__file__))
        # if the script lives in <root>/tools/, parent is the root
        candidate = os.path.dirname(script_dir)
        if os.path.isfile(os.path.join(candidate, "dont_touch.txt")):
            plugin_root = candidate
        else:
            plugin_root = os.getcwd()

    return plugin_root, compat_min, no_clean


# Helpers
def run(args, cwd, check=True, capture=False):
    result = subprocess.run(
        args,
        cwd=cwd,
        text=True,
        capture_output=capture,
    )
    if check and result.returncode != 0:
        if capture:
            print(result.stderr or result.stdout)
        sys.exit(1)
    return result


def run_git(args, cwd, capture=True):
    result = subprocess.run(
        ["git"] + args,
        cwd=cwd,
        text=True,
        capture_output=capture,
    )
    return result.returncode == 0, (result.stdout or "").strip(), (result.stderr or "").strip()


# Steps
def ensure_submodule_initialized(plugin_root, submodule_path):
    git_marker = os.path.join(submodule_path, ".git")
    if not (os.path.isdir(git_marker) or os.path.isfile(git_marker)):
        print("godot-cpp submodule not initialised — initialising now...")
        run(["git", "submodule", "update", "--init", "--recursive"], cwd=plugin_root)
        print("Submodule initialised.")
    else:
        print("godot-cpp submodule already initialised.")


def fetch_and_checkout_master(submodule_path):
    print("Fetching latest remote branches...")
    ok, _, err = run_git(["fetch", "--all"], cwd=submodule_path)
    if not ok:
        print(f"Warning: fetch may have had issues: {err}")

    print("Checking out master branch on godot-cpp...")
    ok, out, err = run_git(
        ["checkout", "-B", "master", "origin/master"],
        cwd=submodule_path,
    )
    if not ok:
        print(f"Failed to checkout master: {err or out}")
        sys.exit(1)

    ok, out, err = run_git(["pull"], cwd=submodule_path)
    if not ok:
        print(f"Warning: pull may have been unnecessary or failed: {err or out}")

    ok, commit, _ = run_git(["log", "--oneline", "-1"], cwd=submodule_path, capture=True)
    print(f"godot-cpp is now at: {commit}")


def update_gitmodules(gitmodules_path):
    try:
        with open(gitmodules_path, "r") as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Warning: {gitmodules_path} not found — skipping .gitmodules update.")
        return

    updated = re.sub(r'(branch\s*=\s*)\S+', r'\g<1>master', content)
    with open(gitmodules_path, "w") as f:
        f.write(updated)
    print(".gitmodules updated: branch = master")


def sync_submodule(plugin_root):
    ok, _, err = run_git(["submodule", "sync"], cwd=plugin_root)
    if not ok:
        print(f"Warning: submodule sync may have failed: {err}")
    else:
        print("Submodule sync done.")


def update_dont_touch(dont_touch_path, new_version="4.6"):
    try:
        with open(dont_touch_path, "r") as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Warning: {dont_touch_path} not found — skipping.")
        return None

    if len(lines) < 2:
        print("Warning: dont_touch.txt has fewer than 2 lines — skipping version update.")
        return lines[0].strip() if lines else None

    lines[1] = new_version + "\n"
    with open(dont_touch_path, "w") as f:
        f.writelines(lines)

    plugin_name = lines[0].strip().lower()
    print(f"dont_touch.txt updated: version -> {new_version}  (plugin: {plugin_name})")
    return plugin_name


def update_gdextension(plugin_root, plugin_name, compat_min):
    # Try the standard locations
    candidates = [
        os.path.join(plugin_root, "test_project", plugin_name, f"{plugin_name}.gdextension"),
        os.path.join(plugin_root, plugin_name, f"{plugin_name}.gdextension"),
    ]
    # Also do a recursive search as fallback
    gdext_path = None
    for c in candidates:
        if os.path.isfile(c):
            gdext_path = c
            break

    if gdext_path is None:
        # Recursive fallback
        for root, _, files in os.walk(plugin_root):
            # skip the submodule
            if "godot-cpp" in root:
                continue
            for fname in files:
                if fname.endswith(".gdextension"):
                    gdext_path = os.path.join(root, fname)
                    break
            if gdext_path:
                break

    if gdext_path is None:
        print("Warning: could not find a .gdextension file — skipping.")
        return

    with open(gdext_path, "r") as f:
        content = f.read()

    updated = re.sub(
        r'compatibility_minimum\s*=\s*"[^"]*"',
        f'compatibility_minimum = "{compat_min}"',
        content,
    )

    with open(gdext_path, "w") as f:
        f.write(updated)

    print(f".gdextension updated: compatibility_minimum = \"{compat_min}\"  ({os.path.relpath(gdext_path, plugin_root)})")


def clean_build(plugin_root):
    print("Cleaning old build files (scons -c)...")
    result = subprocess.run(["scons", "-c"], cwd=plugin_root)
    if result.returncode != 0:
        print("Warning: scons -c returned a non-zero exit code.")
    else:
        print("Old build files cleaned.")


# Main
def main():
    plugin_root, compat_min, no_clean = parse_args()

    print(f"\n=== Upgrading plugin at: {plugin_root} ===")
    print(f"    compatibility_minimum will be set to: {compat_min}")
    print(f"    clean build files: {'no' if no_clean else 'yes'}\n")

    if not os.path.isdir(plugin_root):
        print(f"Error: plugin root not found: {plugin_root}")
        sys.exit(1)

    submodule_path  = os.path.join(plugin_root, "godot-cpp")
    gitmodules_path = os.path.join(plugin_root, ".gitmodules")
    dont_touch_path = os.path.join(plugin_root, "dont_touch.txt")

    ensure_submodule_initialized(plugin_root, submodule_path)
    fetch_and_checkout_master(submodule_path)
    update_gitmodules(gitmodules_path)
    sync_submodule(plugin_root)

    plugin_name = update_dont_touch(dont_touch_path, new_version="4.6")
    if plugin_name:
        update_gdextension(plugin_root, plugin_name, compat_min)

    if not no_clean:
        clean_build(plugin_root)

    print("\n=== Done! Recompile the plugin to apply changes. ===\n")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nCancelled.")
        sys.exit(0)
