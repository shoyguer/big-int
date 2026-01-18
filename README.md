# godot-plus-plus
This repository provides a user-friendly template for developing [GDExtensions](https://docs.godotengine.org/en/stable/classes/class_gdextension.html) in C++ for [Godot Engine](https://godotengine.org/).  Unlike traditional setups that require manual configuration, file editing, and command-line tinkering, [Godot Plus Plus](https://github.com/nikoladevelops/godot-plus-plus) simplifies the entire process with a single <b>`setup.py`</b> script that automates everything - from initializing submodules to configuring your library name and entry points. This makes it perfect for beginners and experienced developers alike, enabling a fast workflow to write, compile, and test C++ plugins in Godot with minimal hassle.

## Who Should Use This Template?
- Perfect for beginners as well as advanced developers that are looking for an easy setup to write C++ code
- Anyone that wants to develop <b>CROSS PLATFORM C++ PLUGINS OR GAMES</b> made in Godot
- Anyone searching for how to update an old GDExtension C++ plugin so that it becomes cross platform
- Anyone searching for how to boost your game's performance - making your games <b>SIGNIFICANTLY FASTER AND MORE OPTIMIZED</b>
- Anyone that doesn't have multiple devices with different operating systems to test the plugin, but still wants everything he writes to be cross platform and to work for any architecture (x86_64, arm64 etc..)

This template supports GitHub Actions - compiling for <b>Windows, macOS, Linux, Android, iOS, macOS and even Web</b>

Compiles both <b>debug</b> as well as <b>final debug + release</b> builds when you are ready to publish your finished plugin to the [Godot Asset Library](https://godotengine.org/asset-library/asset) / [Itch.io](https://itch.io/) or anywhere else you might decide

Compiles <b>threaded</b> and <b>non-threaded Web</b> builds

Applies <b>LTO</b> for the final plugin release to <b>boost performance even further</b>

<b>No complex setups and commands to remember, you just use `setup.py` and you are ready to write C++ GDExtension code for Godot Engine</b>

<b>People just unzip your plugin and it works from the get go - again no hassle for the user, everything works out of the box!</b>

As proof of the capabilities of this template, you can check out [BlastBullets2D](https://github.com/nikoladevelops/godot-blast-bullets-2d) - a fully finished 2D bullets plugin that uses C++ and boosts your game's performance, download the `test_project.zip` to see what it can do or even try forking it to compile and make changes to it yourself

## Requirements
- [GitHub](https://github.com/) account because we are going to be using GitHub Actions for cross platform compilation
- [Git](https://git-scm.com/downloads) installed on your machine and configured correctly so you can push changes to remote
- [Python](https://www.python.org/) latest version and ensure it's available in <b>system environment PATH</b>
- [Scons](https://scons.org/) latest version and ensure it's available in <b>system environment PATH</b>
    - Windows command: `pip install scons`
    - macOS command: `python3 -m pip install scons`
    - Linux command `python3 -m pip install scons`
- C++ compiler
    - Windows: MSVC (Microsoft Visual C++) via Visual Studio or Build Tools.
    - macOS: Clang (included with Xcode or Xcode Command Line Tools).
    - Linux: GCC or Clang (available via package managers).
- [Visual Studio Code](https://code.visualstudio.com/) or any other editor that supports C++ and the `compile_commands.json`

Here are some include directories, if you are stubbornly choosing not to use compile_commands.json or if for some reason your editor needs it for extra features (Visual Studio Code will NOT need it as long as we use the Clangd extension)
```
${workspaceFolder}/godot-cpp/gdextension/
${workspaceFolder}/godot-cpp/gen/**
${workspaceFolder}/godot-cpp/include/**
${workspaceFolder}/godot-cpp/src/**

${workspaceFolder}/src   -> usually where you write all your code  
```




## How to use

You can choose to watch this tutorial for beginners - https://www.youtube.com/watch?v=I79u5KNl34o
##

<b>THIS IS A TEMPLATE, DON'T CLONE THE REPOSITORY YET</b>

1. To use this template, log in to GitHub and click the green <b>"Use this template"</b> button at the top of the repository page (not the clone button).
This will let you create a copy of this repository with a clean git history. Please ensure you set it to public, because GitHub Actions might fail for some linux builds (GitHub will fix this in the future)

2. From now on you will work <b>in your own repository</b> - open it inside your browser, we are gonna make some slight changes

3. Modify the <b>`README.md`</b> file by clearing it and writing something useful about the code you're about to write

4. <b>REPLACE THE `LICENSE` CONTENT</b> with the correct license that your plugin will be in. If you don't know which license to pick, just pick <b>MIT License</b>

5. Clone <b>your own repository</b> that you just made

6. Open <b>`Visual Studio Code`</b> inside the directory you just cloned (where <b>`setup.py`</b> is located)

7. If you have the Microsoft C/C++ extension <b>DELETE IT OR DISABLE IT</b>

8. The extensions that we are going to use for VS Code are the following:

File is called <b>`extensions.json`</b>
```
"recommendations": [
        "llvm-vs-code-extensions.vscode-clangd",
        "ms-python.python",
        "amiralizadeh9480.cpp-helper"
    ]
```

Basically instead of Microsoft's extension for C++, we are going to use the `Clangd` extension which is far superior for intelisense, when you install it, it will offer to install the language server as well, click yes you need that as well


9. Run the <b>`setup.py`</b> script
    - Open your command line terminal where <b>`setup.py`</b> is located (You can use the VS Code terminal too)
    - Run `python setup.py` command inside the terminal to run the script

10. Click Enter and follow the instructions by choosing the options <b>(from 1 to 4 one by one)</b>
#
```
Choose an option
1. Change Godot Target Version
2. Change Build Profile
3. Rename Plugin
4. Compile Plugin Debug Build
5. Generate Missing XML Documentation Files
```

1. Select branch 4.4 since this is the latest version of Godot and is preferred (it will ensure to update it to the latest minor version as well, so picking 4.4 will actually update it to 4.4.1 etc..)
2. Select either 2D or 3D build profile depending on which classes you will be using (select custom if you know what you're doing)
3. Give a name to your plugin
4. Compile (the first time you compile it will take super long, but don't worry after the first time it will be much faster)
5. Skip for now, since we haven't made any changes to the source code, but when you do modify your code and add new functions or properties, it's very useful to run it because <b>this will make documentation available in the editor for all your C++ functions and properties that you want to expose to Godot</b>
#


Warning: The first time you compile and the VS Code project is open, the Clangd extension will try to read and cache a lot of the information about your classes that were generated inside compile_commands.json by beginning to index them (you will actually notice that at the bottom left of your Visual Studio Code window). So before beginning to write code, please wait for everything to finish.

If everything went well, then every time you type a godot class (example : `Sprite2D`), you should get intelisense as well as auto header includes. 
Usually header includes that come from an external library should be with angle brackets, but even if you leave the godot-cpp headers with double quotes, it's still fine so don't worry about it


Note: I've excluded a lot of the files from the explorer that are unnecessary, but you can always unhide some of them by going inside `.vscode/settings.json` and modifying the values there from true to false


Warning: If you are on Linux or macOS and see <b>any weird errors that don't make sense (like seeing detected errors on comments)</b>, but your code compiles perfectly fine, then ensure you have a `.clangd` file inside your VS Code directory and paste this inside if it's not already there:

```
CompileFlags:
  Add: -Wno-unknown-warning-option
  Remove: [-m*, -f*]
```

Also, instead of VS Code, you could try using [Zed](https://zed.dev/), it is insanely fast and it works with Clangd by default. Soon Zed is coming to Windows too.

## Some advice
1. Don't modify the godot-cpp classes, always make your own - you can choose to inherit from theirs or use pure C++ classes (pure classes won't be exposed to Godot)
2. If you need source code of some classes, check [Godot Engine's source code](https://github.com/godotengine/godot)
3. Godot uses C++ 17 currently, so keep it that way for compatibility
4. Read the source code of `item_data.hpp` and `item_data.cpp` to understand how everything works - I've written lots of useful information as comments there, so whether you are a beginner or an advanced developer it will be insanely helpful to you
5. When making brand new classes ensure you use `GDCLASS`, and `bind_methods()` and finally ensure you register the actual class inside `register_types.cpp` or else it won't be visible inside the editor
6. Usually when you are writing C++ code, the `test_project` should be open - you write some C++ code, then compile and you repeat that over and over. Note that sometimes you might not see some of the changes, so you need to restart the Godot project <b>(Project -> Reload Current Project)</b>
7. You can edit the ```.github/workflows/build-plugin.yml``` file and add or remove operating systems and architectures for which you want to compile your plugin

## Does My Plugin Work On Other Operating Systems?
Right now every time you compile your code, it is being compiled for your own operating system and your own architecture, but since you want your plugin to be used by other people that might be on a different operating system (whether they are using the editor or trying to export their game with your plugin) we need to use GitHub Actions to test if everything is working correctly


Every time you've added features to your plugin and you are wondering if it works for other operating systems you do this:

1. Commit and Push your code to remote
2. Go to the GitHub Actions tab on your repository
3. Run the <b>"Build GDExtension Cross Platform Plugin"</b> workflow by selecting that you want a `debug` build only - again first time compiling will be slow, but GitHub Actions also uses cache for Scons that lasts around 7 days, so it's fine. After everything is completed you will see a `finished_unzip_me` that contains a zip of your plugin. If everything is green, congrats your plugin works on all operating systems, if however you see red, it means that some of the builds failed and you need to play around and see why and fix your issues, then push to remote and try again.. repeat..

<b>Warning:</b> You should not use these builds as a plugin release.. they only test if everything is compiling correctly instead of wasting more resources for optimizations


## My C++ GDExtension Plugin Is Ready, I Want To Publish It
If you compiled a debug build first, and you saw that everything is working and all your features are truly done, then you can go ahead and do your full plugin compilation that compiles `debug` builds used by the editor as well as `release` builds that are meant to be used by the exported game of the user.

To do this, go inside your repository, then go to Actions tab, and then again run the <b>"Build GDExtension Cross Platform Plugin"</b> workflow, but this time from the dropdown instead of `debug`, you should choose `full_plugin_compilation`.

Again a `finished_unzip_me` that contains a zip of your plugin will be generated.

1. Unzip it to get the actual zip for publishing
2. Publish your plugin zip file as a release on your GitHub Repository / Godot Asset Library / Itch.io or give it to someone to test


## How Does Someone Download And Install The Plugin?

1. Download the zip
2. Unzip it inside a Godot project

That's all! No matter if the user unzips it inside an "addon" folder or leaves it in the root directory or inside another folder, the plugin will still work.

## Support
If you wish to support me you can do so here - https://ko-fi.com/realnikich or https://patreon.com/realnikich

If you find this template useful:
- <b>Leave a Star on the repository</b>
- Expect <b>GDExtension Tutorials</b> on my YouTube channel - https://www.youtube.com/@realnikich
- [Follow me on X (Twitter)](https://x.com/realNikich)
- [Follow me on Bluesky](https://bsky.app/profile/realnikich.bsky.social)

This template wouldn't be possible without the offical [godot-cpp](https://github.com/godotengine/godot-cpp) repository as well as the [godot-cpp-template](https://github.com/godotengine/godot-cpp-template), so you can star them as well
