# SpecsHD

Spelunky HD Debugging DLL

This repository is meant only for the development of SpecsHD.
Access for non-developers is provided via [HD Toolbox](https://github.com/spelunky-fyi/HD-Toolbox).

# Setup Dependencies

Dependencies are either vendored in the repo or provided via git submodules.
Before building you'll need to pull the submodules:

```shell
git submodule update --init --recursive
```

# DLL

## Initial Setup

```
cd specs
cmake -A Win32 -S . -B "build"
```

## Rebuilding

```
cmake --build build --config Release
```

# Injector

You should be able to use any injector however an injector is provided for local development / testing.
It has to be built as a 32-bit executable to properly inject the dll so you'll need a 32-bit toolchain.

## Install 32-bit Toolchain

```
rustup install stable-i686-pc-windows-gnu
```

## Run Injector

```
cd injector
cargo +stable-i686-pc-windows-gnu run --release -- ..\specs\build\Release\specs.dll
```

## Build Injector

If you want to build the injector your can run

```
cargo +stable-i686-pc-windows-gnu build --release
```

# Troubleshooting


### Not seeing tool window

The hook for dx9 seems finnicky and I believe it can conflict with the Steam overlay to some degree. I have the best luck when injecting as soon as possible before the steam popups occur or after the game has already been running for a minute. If you attempt to inject the dll and don't see the tool window then it likely didn't work and Spelunky will need to be restarted to try again. The provided injector will wait and inject as soon as the game launches and works best when run before launching the game.

### Seeing a link error when building the DLL

This happens if you've injected the DLL and the game is still running as the linker can't overwrite the DLL while it is still in use.
