# Seamless Saving - Skyrim Save Accelerator

An SKSE plugin that eliminates save lag by heavily optimizing the Skyrim Script VM save process — reducing script VM save time by up to **~89%** and total save time by up to **~60%**.

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

### Requirements

- [xmake](https://xmake.io) 2.8.2+
- Visual Studio 2022 with C++ workload (MSVC)

### Steps

```bash
git clone https://github.com/JerryYOJ/Seamless-Saving-SKSE.git
cd Seamless-Saving-SKSE
git submodule update --init --recursive
xmake
```

xmake will download all required packages automatically and generate `xmake-requires.lock` on the first run.

### Auto-deploy

Set the `SkyrimPluginTargets` environment variable to one or more target directories (semicolon-separated) and the built `.dll`/`.pdb` will be copied to `<dir>/SKSE/Plugins/` automatically after each build.

```
SkyrimPluginTargets=C:\path\to\skyrim\Data;C:\path\to\MO2\mods\SeamlessSaving
```

## License

[GPL-3.0-or-later](COPYING) WITH [Modding Exception AND GPL-3.0 Linking Exception (with Corresponding Source)](EXCEPTIONS.md).
Specifically, the Modded Code is Skyrim (and its variants) and Modding Libraries include [SKSE](https://skse.silverlock.org/), Commonlib (and variants), and Windows.
