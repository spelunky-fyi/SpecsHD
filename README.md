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
cmake -A Win32 -S -B "build"
```

## Rebuilding

```
cmake --build build --config Release
```

# Releasing

Releases are cut by the [release workflow](.github/workflows/release.yml),
which fires automatically when a push to `main` changes the `VERSION` file.
It reads the version, builds `specs.dll` with MSVC x86, tags `v<version>`,
and publishes a GitHub release with `specs.dll` attached and the matching
`CHANGELOG.md` section as the body.

To cut a new release:

1. Make sure everything you want shipped is on `main` and the
   `## Unreleased` section of [`CHANGELOG.md`](CHANGELOG.md) describes it.
2. In `CHANGELOG.md`, rename `## Unreleased` to `## [X.Y.Z] - YYYY-MM-DD`
   (today's date) and add a fresh empty `## Unreleased` section above it
   for the next cycle.
3. Bump [`VERSION`](VERSION) to `X.Y.Z` (bare version, no `v` prefix, no
   trailing newline rules -- the workflow trims whitespace).
4. Commit (`cut release X.Y.Z` is the conventional message) and push to
   `main`.

The workflow runs on the resulting push. If a `v<version>` tag already
exists it no-ops, so re-pushing the same version is safe but won't
republish; bump `VERSION` again to retry.

If you need to inspect or redo a release, the tag and release are both
under the `v<version>` name on GitHub. Deleting both lets the workflow
recreate them on the next `VERSION`-touching push.

# Troubleshooting

### Seeing a link error when building the DLL

This happens if you've injected the DLL and the game is still running as the linker can't overwrite the DLL while it is still in use.
