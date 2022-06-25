# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Added

- Level Map view in the Level Tab

## [0.0.6] - 2022-06-25

### Fixed

- Remove button not working for some exclusions
- Don't search for entity if null
- Dragging fixed when Clostest hitbox/id is used
- Player won't take fall damage when released from dragging

## [0.0.5] - 2022-06-24

### Added

- Support for dragging selectable entities is now supported just hold middle click to enable dragging.

### Changed

- Reduced select distance from 2.5 to 1 as selection was happening from a pretty unintuitive distance.
- Selecting in a place with no selectable entity now clears the selection
- Raw Bytes in the Selection view now uses a table for easier visibility

#### Fixed

- ID/Hitbox/etc alignments now work in borderless window mode
- Fixed issue where overlay wouldn't be rendered in some scenarios
- Bug where exclude didn't work on IDs and Selection settings

## [0.0.4] - 2022-06-23

Welcome Specs HD's second contributor Estebanfer! Thanks!

### Added

- Global State Tab that includes various flags and details from Global State
- Selected Tab that displays details about a selected entity
- UI Entities added to Debug

### Changed

- Debug tab now has collapsable menus to help with organization for hitbox/id/selection config

## [0.0.3] - 2022-06-21

### Changed

- Broke up UI into various tabs
  - Debug Overlays in `Debug` tab
  - Spawning in `Spawn` tab
- Completely redid dx hooking / imgui setup which now handles fullscreen/resolution changes/late loading
- Re-did local injector to work as both 32-bit and 64-bit which simplifies development.

### Added

- `Players` tab with ability to set health/bombs/ropes and items for each player
- `Levels` tab with ability to jump to a level
- `Audio` tab with ability to play audio effects

## [0.0.2] - 2022-06-20

### Added

- Ability to spawn entities
- Ability to teleport (Right Click)
- Pacifist Overlay
- Hitboxes

### Changed

- Updated names for some entity lists to be more descriptive

## [0.0.1] - 2022-06-19

### Added

Initial release that contains the following:

- Drawing Grid Outlines
- Drawing Bin Borders
- Drawing Entity IDs from various Entity arrays
- Excluding specific Entity IDs from being drawn.
