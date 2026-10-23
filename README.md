# Def Jam: Fight for NY XBE Recomp

An unofficial native Linux port of **Def Jam: Fight for NY** (Xbox, 2004) by static recompilation. The game's x86 XBE is translated to C++ ahead of time and the Xbox kernel and XDK libraries (Direct3D 8, DirectSound, XAPI) are implemented at the API level. It draws with Vulkan and plays sound and reads input through SDL3. There is no emulator, pushbuffer interpreter or software rasteriser.

Based on [TGP482/XboxXBERecomp](https://github.com/TGP482/XboxXBERecomp). See [Credits and licensing](#credits-and-licensing).

> [!NOTE]
> This repository does not include any game files or recompiled game code. You need your own legally acquired copy of the game to build and run it.

## Status

| | |
| --- | --- |
| title | Def Jam: Fight for NY (USA), title id `0x45410049`, XDK 5849 |
| boots | yes, to the intro movies, front end and fights |
| renderer | Vulkan 1.4 (shaders compiled to SPIR-V at run time by shaderc) |
| audio | SDL3, stereo, 48 kHz |
| input | SDL3 controllers, and the keyboard as controller 1 |
| game speed | 60 Hz logic, as on the console |
| online | not supported (XNet/XOnline are not implemented) |

Gameplay is still being tested. `SUMMARY.md` has what was fixed to get here and what is open, and `NOTES.md` covers how things work.

## Requirements

- Linux with Wayland or X11 and a Vulkan 1.3+ GPU
- `clang`, `lld`, `cmake`, `ninja`, `git`, `pkg-config`
- Vulkan headers and loader, and `shaderc`
- SDL3's build dependencies. CMake fetches SDL3, Zydis, toml++ and Dear ImGui.

Arch:

```bash
sudo pacman -S clang lld cmake ninja git pkgconf vulkan-headers vulkan-icd-loader shaderc
```

`build.sh` lists the Debian/Ubuntu and Fedora packages if something is missing.

## Building

1. Put the game in `assets/`: either the disc image (`.iso`), or `default.xbe` with its data folders (`assets`, `audio`, `fonts`, `motion`, `movies`, `screens`, ...) beside it.
2. Run:

   ```bash
   ./build.sh          # release
   ./build.sh debug    # same optimisation, full logging
   ```

   The first build finds the XDK symbols, recompiles `default.xbe` into `generated/`, and compiles it, which takes from a few minutes to an hour depending on the machine. Later builds are incremental.
3. Run `out/build/release/defjam`.

> [!IMPORTANT]
> `config/game.toml` carries symbols mapped by hand on top of the generated ones (`BeginPush`, `EndPush`, `SetPalette` and seven DirectSound buffer functions). `build.sh` keeps the file once it has a `[symbols]` section. Do not delete or regenerate it, or those fixes are lost.

## Controls

A controller works as on the Xbox. The keyboard always plays as controller 1:

| Keyboard | Xbox controller |
| --- | --- |
| Arrow keys | D-pad |
| W A S D | Left stick |
| Numpad 8 4 2 6 | Right stick |
| Enter | Start |
| Esc / Backspace | Back |
| Space / J | A |
| K | B |
| U | X |
| I | Y |
| O | Black |
| L | White |
| Q / E | Left / right trigger |
| Left / right Shift | Left / right stick click |

Keys are set in `src/common/keyboard.cpp`.

## Diagnostics

- Logs go to `out/build/release/logs/defjam.log`. A release build writes errors and reports only, while a debug build writes everything.
- **Frame capture:** start with `XBE_CAPTURE_DIR=/tmp/djcap ./defjam`, then press F3, or run `kill -USR1 $(pgrep -x defjam)`. The next frame's draws, render targets and textures are saved under that folder.
- `XBE_PERF=1` logs frame timing.

## Layout

| path | |
| --- | --- |
| `build.sh` | toolchain check, symbols, cmake + ninja |
| `config/game.toml` | recompiler input: sections, entry points, XDK symbol map |
| `src/d3d/` | Vulkan backend for Linux and its headers |
| `src/audio/sdl_linux.cpp` | audio output through SDL3 |
| `src/common/` | game info, keyboard mapping, capture signal, Linux versions of Windows-only SDK files |
| `src/game/hooks.cpp` | title-specific function overrides |
| `tools/match_symbols.py` | borrows symbol names from another title linking the same XDK (last resort) |
| `sdk/` | XboxXBERecomp, with this port's runtime fixes |

## Credits and licensing

- [TGP482](https://github.com/TGP482) for [XboxXBERecomp](https://github.com/TGP482/XboxXBERecomp), the recompiler and runtime SDK this port is built on, kept in `sdk/` with this port's runtime fixes.
- The Vulkan renderer came from my BLACK fork for Linux, [datfooldive/BLACK-XBERecomp-Linux](https://github.com/datfooldive/BLACK-XBERecomp-Linux), forked from [TGP482/BLACK-XBERecomp](https://github.com/TGP482/BLACK-XBERecomp).
- The libraries and references credited in `sdk/README.md`.

The SDK is MIT licensed, see `sdk/LICENSE` and `sdk/THIRD_PARTY.txt`.

This repository contains no game files or recompiled game code. Def Jam: Fight for NY is © 2004 Electronic Arts Inc., developed by AKI Corporation. This project is not affiliated with or endorsed by Electronic Arts, AKI Corporation or Def Jam.
