# BigNumber
A high-performance Big Number library for Godot games. Perfect for idle / incremental games, or any project where you need numbers exceeding the limits of standard integer and float types.

![Header Image](https://raw.githubusercontent.com/shoyguer/big-int/refs/heads/main/brand/header_image.png)

This plugin was built upon ChronoDk's [GodotBigNumberClass](https://github.com/ChronoDK/GodotBigNumberClass), credits to him!
Though my version uses GDExtension instead, for optimization.

Check the comparison below (results may vary):
![Testing Results](https://raw.githubusercontent.com/shoyguer/big-int/refs/heads/main/brand/benchmark.png)
Hardware used for testing:
- AMD Ryzen 5 5600
- AMD RX 6750XT
- 48 GB RAM 3200MHz
- 512 GB SSD M.2

## Why Use BigNumber?
Managing massive numbers in games can be performance-heavy and complex. This plugin provides you with:
- **Infinite Scaling**: Capable of handling numbers much larger than `float` or `int64`.
- **High Performance**: Written in C++ to be significantly faster than GDScript implementations.
- **Rich Formatting**: Built-in support for multiple notation styles:
    - Scientific (1.23e45)
    - AA Notation (aa, ab, ac...)
    - Metric Symbols (k, M, B, T...)
    - Metric Names (kilo, mega, giga...)
- **Easy-to-use API**: Methods similar to standard arithmetic operations.

This is a GDExtension plugin, built with performance in mind.

### Target platforms:
| Platform | Supported Systems |
|----------|------------------|
| **Desktop** | 🪟 Windows • 🐧 Linux • 🍎 MacOS |
| **Mobile** | 🤖 Android • 📱 iOS |
| **Others** | 🌐 Web |

## Requirements
- [Godot 4.5](https://godotengine.org/)

## Building From Source
Only needed if you want to modify the plugin.
Follow [godot-plus-plus](https://github.com/nikoladevelops/godot-plus-plus/tree/main) instructions, and you'll be fine!

## Installation
1. Download the latest release
2. Extract to your Godot project, and make sure this is how your project structure looks:
```
your_project_folder/
├── addons/
│   └── big_number/
│       ├── bin/
│       ├── big_number.gdextension
│       └── ...
```
3. That's it! **BigNumber** should now be installed :grin:.

(Since it is a GDExtension plugin, you don't need to activate it through Project Settings. Just restart the editor if classes don't appear immediately.)

## How to use BigNumber
Check the [Wiki](https://github.com/shoyguer/seed/wiki) for information on how to use this plugin.

Or jump to whatever you need:

#### Wiki
- [Class Reference](https://github.com/shoyguer/seed/wiki/1.-Class-Reference)
- [Code Examples](https://github.com/shoyguer/seed/wiki/2.1.-GDScript-Code-Example)

## Other Plugins
Check out my other Godot plugins:

| Plugin | Description |
|--------|-------------|
| [<img src="https://raw.githubusercontent.com/shoyguer/time-tick/refs/heads/main/brand/header_image.png" width="192">](https://github.com/shoyguer/time-tick) | Flexible time management & tick system |
| [<img src="https://raw.githubusercontent.com/shoyguer/stat-pool/refs/heads/main/brand/header_image.png" width="192">](https://github.com/shoyguer/stat-pool) | Flexible stat management system (for Health, Mana, Stamina, money etc) |
| [<img src="https://raw.githubusercontent.com/shoyguer/seed/refs/heads/main/brand/header_image.png" width="192">](https://github.com/shoyguer/seed) | A simple Random seed generation system |

## Support
If this plugin helped you, please, consider:
- ⭐ Star this repository
- 🐛 Report bugs in Issues
- 💡 Suggest improvements

___

Thanks to [ChronoDK](https://github.com/ChronoDK) for his [GodotBigNumberClass](https://github.com/ChronoDK/GodotBigNumberClass) which inspired and was the base for this plugin.
And a BIG thanks to [Nikich](https://github.com/nikoladevelops) for his [godot-plus-plus](https://github.com/nikoladevelops/godot-plus-plus) template.
