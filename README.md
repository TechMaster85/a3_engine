# A3 Engine

An implementation of the [A2 Engine](https://a2engine.org/) built over a semester for EECS 404 at the University of Michigan. A3 is a 2D game engine with a Lua scripting API, Box2D physics, SDL2 rendering, and a data-oriented particle system.

Three example games are available on [a2engine.org](https://a2engine.org/): *The Whispering Crown*, *Donna the Donut Pilot*, and *Physics Obstacle Course*. You can run any of them by dropping their assets into the `resources/` folder at the root of this repository. Write your own game the same way — all scripts go in `resources/component_types/` as Lua files. See the [scripting documentation](https://docs.google.com/document/d/1sWtp5iNj1Plko7Ps5KJpbjI9Q20iTuIfWqYaNGgke64/edit?tab=t.0#heading=h.fz7isn46xizf) for the full API reference.

## Features

- **Lua scripting** — component-based actor system; scripts in `resources/component_types/`
- **Box2D physics** — rigid bodies, colliders, triggers, raycasting
- **Particle system** — data-oriented (SOA layout), deterministic, Lua-controllable
- **Event system** — publish/subscribe with deferred subscription changes
- **Nintendo Switch** — custom feature; runs natively on Switch hardware via the devkitpro toolchain

## Tech stack

| Dependency | Version | Role |
|---|---|---|
| SDL2 + SDL2_image/ttf/mixer | 2.x | Rendering, audio, input |
| Lua | 5.4 | Scripting language |
| LuaBridge | 2.7 | C++/Lua binding |
| Box2D | 2.4.1 | 2D physics (source-compiled) |
| RapidJSON | — | Config and scene parsing |
| GLM | — | Math types |

## Building

### Linux

**Ubuntu / Debian**
```bash
sudo apt install cmake build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
```

**Fedora**
```bash
sudo dnf install cmake gcc-c++ SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel
```

**Arch Linux**
```bash
sudo pacman -S cmake base-devel sdl2 sdl2_image sdl2_ttf sdl2_mixer
```

```bash
cmake -B build
cmake --build build
./game_engine
```

### macOS

Requires [Homebrew](https://brew.sh/).

```bash
brew install cmake sdl2 sdl2_image sdl2_ttf sdl2_mixer
cmake -B build
cmake --build build
./game_engine
```

### Windows

SDL2 libraries are fetched automatically by CMake. Requires Visual Studio 2019 or later with the **Desktop development with C++** workload. Run the following from a [Developer Command Prompt](https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell):

```powershell
cmake -B build
cmake --build build --config Release
game_engine.exe
```

### Nintendo Switch

Requires the devkitPro toolchain. Follow the [Getting Started guide](https://devkitpro.org/wiki/Getting_Started) to install it, then add the `switch-dev` group and SDL2 ports:

```bash
dkp-pacman -S switch-dev switch-sdl2 switch-sdl2_image switch-sdl2_ttf switch-sdl2_mixer
```

Before building, confirm `$DEVKITPRO` is set — the install method varies by OS:

```bash
echo $DEVKITPRO   # should print something like /opt/devkitpro
```

If it's empty, set it manually (adjust the path to match your install):

```bash
export DEVKITPRO=/opt/devkitpro
```

Then build:

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/cmake/Switch.cmake
cmake --build build
```

`game_engine.nro` is placed in the project root, ready to run with a homebrew launcher or an emulator (if you can find one).
