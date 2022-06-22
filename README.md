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

## Run Injector

```
cd injector
cargo --release -- ..\specs\build\Release\specs.dll
```

## Build Injector

If you want to build the injector your can run

```
cargo build --release
```

# Troubleshooting

### Seeing a link error when building the DLL

This happens if you've injected the DLL and the game is still running as the linker can't overwrite the DLL while it is still in use.
