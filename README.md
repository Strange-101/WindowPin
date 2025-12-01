# WindowPin

**WindowPin** is a lightweight Windows tool that allows you to pin any window on top of others, with optional opacity and click-through functionality. Perfect for coding alongside videos, tutorials, or reference materials.

> **Note:** Currently, WindowPin is **console-based**. It is a work in progress and will be developed into a full application with a GUI in the future.

---

## Features

* Pin any window on top of others.
* Toggle semi-transparent mode for pinned windows.
* Toggle click-through to let clicks pass through the pinned window.
* Snap pinned windows to corners for quick multitasking.
* Fully controllable via customizable hotkeys.

---

## Hotkeys (Default)

| Hotkey           | Action                                    |
| ---------------- | ----------------------------------------- |
| Ctrl + Shift + P | Pin the active window                     |
| Ctrl + Shift + U | Unpin / restore window                    |
| Ctrl + Shift + O | Toggle opacity (semi-transparent)         |
| Ctrl + Shift + C | Toggle click-through                      |

---

## Installation

1. Download the latest release from GitHub.
2. Compile `main.cpp` using Code given below.
3. Run `main.exe`. A console window will appear showing the hotkeys.
4. Focus any window you want to pin and use hotkeys to control it.

> **Note:** No installation is required. The tool is standalone.

---

## Build from Source

Requirements:

* Microsoft Visual Studio 2022 Build Tools
* Windows SDK

Compile using the Developer Command Prompt:

```cmd
cl.exe /EHsc /O2 /MD main.cpp user32.lib gdi32.lib dwmapi.lib
```

This produces a `main.exe` ready to run.

---

## License

MIT License. Free to use, modify, and distribute.

---

## Contributing

Feel free to fork this repository, submit bug reports, or propose new features. Pull requests are welcome!
