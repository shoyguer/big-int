# BigNumber
A high-performance Big Number library for Godot games. Perfect for idle / incremental games, or any project where you need numbers exceeding the limits of standard integer and float types.

![Header Image](https://raw.githubusercontent.com/shoyguer/big-int/refs/heads/main/brand/header_image.png)

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
- [Godot 4.3+](https://godotengine.org/)

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
Initialize a BigNumber from various types:
```gdscript
var num1 = BigNumber.new(100)           # From int
var num2 = BigNumber.new(123.456)       # From float
var num3 = BigNumber.new("1.5e20")      # From String (scientific)
var num4 = BigNumber.new(1.5, 20)       # From mantissa and exponent (1.5 * 10^20)
```

Perform arithmetic:
```gdscript
# Operations return a NEW BigNumber (immutable-style)
var sum = num1.plus(num2)
var diff = num3.minus(num4)
var prod = num1.multiply(num2)
var quot = num3.divide(num4)

# In-place operations (modifies the object)
num1.plus_equals(50)
num1.multiply_equals(BigNumber.new("1e5"))
```

Comparisons:
```gdscript
if num1.is_greater_than(num2):
    print("Num1 is bigger!")
    
if num3.is_equal_to(num4):
    print("They are equal")
```

Formatting:
```gdscript
print(num3.to_scientific())     # "1.50e20"
print(num3.to_aa())             # AA notation (e.g. "1.50 aa")
print(num3.to_metric_symbol())  # Metric (e.g. "150 E")
print(num3.to_metric_name())    # Full name (e.g. "150 exa")
```

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

And a BIG thanks to [Nikich](https://github.com/nikoladevelops) for his [godot-plus-plus](https://github.com/nikoladevelops/godot-plus-plus) template.
