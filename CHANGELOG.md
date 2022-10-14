# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Added

- Debug Option to see Olmec's Crush Probes
- EntityFloor menu when selecting floors that shows flag for creating door

## [0.0.12] - 2022-10-03

### Updated

- Entity Positions to have more precision

## [0.0.11] - 2022-10-03

### Added

- Ability to disable Olmec spawns from Debug tab

## [0.0.10] - 2022-09-29

### Added

- Support for scaling the UI in the Settings tab (persisting this will come later)
- Support for locking Player Data passives
  - Note: Locked ankh will result in end adventure procing the ankh
  - Note: Back Items will be re-persisted across level transitions

## [0.0.9] - 2022-09-28

### Added

- Support for setting Respawn Level
- Warping between levels now reset the level timer
- Ability to spawn multiple entities at once

## [0.0.8] - 2022-06-29

### Added

- Settings tab that displays current key bindings so far

### Changed

- All Key/Mouse inputs are handled through a central config to support user bindings
- Moved GlobalState views into Debug tab to make room on tab bar
- Changed default bind of `Frame Advance` to `PageDown`

## [0.0.7] - 2022-06-27

### Added

- Level Map view in the Level Tab
- Room Type view in the Level Tab
- Support for floor decorations (over, top, bottom, left, right) in Debug
- Populated more fields for LevelState
- Spawn Tab
  - Selectable Entity List
  - Filter for Entity List
  - Toggle for spawning Entity into Active Entity array (parameter of spawn_entity)
  - Click and Drag to spawn with velocity
- Added missing sound effects to Audio Tab
- Frame Advance
  - `Ctrl` + `Space` - Pause Game Engine
  - `Shift` + `Space` - Advance Frame (Can be held for repeated advancement)

### Changed

- Audio tab now uses a list box instead of a bunch of different sized buttons which looks nicer

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
