# Seamless Saving - Skyrim Save Accelerator

An SKSE plugin that eliminates save lag by heavily optimizing the Skyrim Script VM save process — reducing script VM save time by up to **~89%** and total save time by up to **~60%**.

- [Nexus SSE/AE](https://www.nexusmods.com/skyrimspecialedition/mods/173161)
- [VR](https://www.nexusmods.com/skyrimspecialedition/mods/174106)

## The Problem

Save lag is a persistent immersion breaker in every Skyrim play session. A typical save takes around 200ms — just long enough to cause an annoying freeze. The biggest bottleneck is saving the Script VM, which accounts for **56% of total save time**.

## How It Works

Seamless Saving attacks this bottleneck with two techniques:

- **Multithreading** — the entire VM save process is lifted to a background thread, running in parallel with the rest of the save
- **String table caching** — the script string table is cached and lazy-loaded at save time, eliminating redundant work on every save

## Requirements

- [Skyrim Special Edition](https://store.steampowered.com/app/489830) (SE or AE)
- [SKSE64](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Installation

Install with a mod manager (MO2, Vortex, etc.) or manually copy the `.dll` and `.pdb` from `SKSE/Plugins/` into your Skyrim `Data/SKSE/Plugins/` folder.

## Building from Source

Both xmake and cmake/vcpkg build systems are supported.

### Common requirements

- Visual Studio 2022 with C++ workload (MSVC)
- Git (for cloning and submodules)

```bash
git clone https://github.com/JerryYOJ/Seamless-Saving-SKSE.git
cd Seamless-Saving-SKSE
```

---

### xmake

**Additional requirements:** [xmake](https://xmake.io) 2.8.2+

```bash
git submodule update --init --recursive
xmake
```

xmake downloads all required packages automatically and generates `xmake-requires.lock` on the first run.

**Auto-deploy:** set one or more of these environment variables and the `.dll`/`.pdb` will be copied to `SKSE/Plugins/` after every build:

| Variable              | Destination                                      |
| --------------------- | ------------------------------------------------ |
| `SkyrimPluginTargets` | `<dir>/SKSE/Plugins/` (semicolon-separated list) |
| `SKYRIM_MODS_FOLDER`  | `<dir>/Seamless Saving/SKSE/Plugins/`            |
| `SKYRIM_FOLDER`       | `<dir>/Data/SKSE/Plugins/`                       |

---

### cmake

The CMake build now uses the copy of CommonLibSSE stored under `lib/commonlibsse-ng` (the submodule) instead of the version provided by
vcpkg. You do **not** need to install the package in vcpkg, although
vcpkg support is still recognised for other libraries if you have it set up.

**Additional requirements:** [CMake](https://cmake.org) 3.21+ (vcpkg is
optional and only used if `VCPKG_ROOT` is defined)

```bash
cmake --preset releasewithdeb -S .buildenv -B .buildenv/build/releasewithdeb
cmake --build .buildenv/build/releasewithdeb
```

Available presets: `debug`, `release`, `releasewithdeb`

**Auto-deploy:** set one of these environment variables before configuring:

| Variable             | Destination                          |
| -------------------- | ------------------------------------ |
| `SKYRIM_MODS_FOLDER` | `<dir>/SeamlessSaving/SKSE/Plugins/` |
| `SKYRIM_FOLDER`      | `<dir>/Data/SKSE/Plugins/`           |

## License

[GPL-3.0-or-later](COPYING) WITH [Modding Exception AND GPL-3.0 Linking Exception (with Corresponding Source)](EXCEPTIONS.md).
Specifically, the Modded Code is Skyrim (and its variants) and Modding Libraries include [SKSE](https://skse.silverlock.org/), Commonlib (and variants), and Windows.
