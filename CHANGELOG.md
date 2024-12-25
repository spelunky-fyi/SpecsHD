# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Added

- Added support for logging shop items while seed finding

## [0.0.44] - 2024-12-23

### Added

- Ability to run internal resetForRun and resetForLevel functions
- Update warp to use resetForLevel before warping
- Add ability to advance to a specific level in seeded mode. Will run through
  all previous levels to simulate going through each level
- Added button to quickly reset run\
- Seed Scan option that will scan your advance level and write crate contents to crates.txt

### Fixed

- Seeds were improperly not multiplying by the level causing level gen to be
  very similar between levels in an area

## [0.0.43] - 2024-12-22

### Updated

- Hopefully better seed gen

## [0.0.42] - 2024-12-22

### Added

- Ability to override seeds for individual levels

## [0.0.41] - 2024-12-04

### Added

- Beta version of Uplunky (don't expect much here)
- Ability to set camera to follow selected entity (Only works on enemies currently)

## [0.0.40] - 2024-04-27

### Added

- Ability to draw lines between follower/following Player/HH links
- Option for Seeded to choose new seed on reset

## [0.0.39] - 2023-10-07

### Added

- Ability to Spawn HH

## [0.0.38] - 2023-09-28

### Added

- Detection boxes for yama's head and hands

## [0.0.37] - 2023-03-07

### Changed

- Seed in seeded mode no longer prevents certain inputs, rollover be damned. 0 -> UINT32_MAX are allowed.

## [0.0.36] - 2023-03-02

### Added

- Overlay that shows seeded crate contents in Debug Tab
- Overlay that shows seeded Kali Rewards in Debug Tab
- Overlay that shows seeded Pot drops in Debug Tab

### Fixed

- Seeded Mode now properly (famous last words) dark levels.

## [0.0.35] - 2023-02-24

### Added

- "Use Daily Seeding" Checkbox for Seeded Mode that emulates Daily shop behavior

## [0.0.34] - 2023-02-19

### Added

- New Mod: Seeded Mode

### Fixed

- Fixed/Added more Entity names for Render Duper view (Thanks zutklown)

## [0.0.33] - 2023-02-16

### Fixed

- Crash on shutdown related to entity hooking
- Tunnel Man Mod now works with crysknife
- Tunnal Man Mod no longer plays audio when you press whip in the air

## [0.0.32] - 2023-02-15

### Added

- New Mod: `Tunnel Man`

## [0.0.31] - 2023-02-15

### Added

- New Mod: `Biglunky`
- Support for `Dark Mode` and `The Full Spelunky` to work together
- `Show Room Borders` in Debug Menu
- `Level State` in Debug Menu that shows various entrance/exit values
- 10 Tile Radius around player in `BM Trainer` to show where BM will be when you hear first chirp

### Changed

- Room Types drop down now shows all 48 rooms always.

### Fixed

- Crash when player is destroyed

## [0.0.30] - 2023-02-12

### Changed

- In The Full Spelunky the player now gives off more brightness by default in dark levels and further more brightness for both specs and the udjat eye.

## [0.0.29] - 2023-02-11

### Changed

- The Full Spelunky now has more guaranteed Cog/Hell coffins

## [0.0.28] - 2023-02-11

### Changed

- Moved Audio tab into collapsed header in Debug
- Room Type View now shows 5 rows instead of 4 in non-worm levels

### Added

- Mods Tab
- "Dark Mode" Mod
- "The Full Spelunky" Mod

## [0.0.27] - 2023-01-29

### Fixed

- Bug related to locking HH Count and Held Item Metadata

## [0.0.26] - 2023-01-25

### Changed

- Black Market Traner now hooks the eligible floors array to get the exact list instead of using a heuristic
- Yellow spaces are spaces there were eligible but were replaced by traps so they appear ineligible

## [0.0.25] - 2023-01-24

### Added

- Black Market Trainer in Debug menu. Shows all places a black market can spawn in Jungle

### Fixed

- Precision on \_z displays

## [0.0.24] - 2023-01-17

### Added

- Render Dupe dropdown in Debug tab that lists active entities along with their current z

### Fixed

- Detection Ray size from enemies to be more accurate
- Precision on z values in selected tab

## [0.0.23] - 2023-01-15

### Added

- Various values related to shopkeeper aggro in Debug->Global State menu
- Held Item Metadata to Player tab
- Hired Hand Count to Player tab

## [0.0.22] - 2023-01-10

### Added

- Support for locking Held Item ID on the player tab.

## [0.0.21] - 2023-01-10

### Fixed

- Explosion Floor Overlay was smaller than expected.

## [0.0.20] - 2023-01-10

### Added

- Landmine support for detection boxes
- Added inner circle for bomb detection that indicates interaction with entity hitboxes
- Overlay red on which floor tiles will be destroyed from explosions
- Overlay red on entites which will be hit by an explosion
- Added support for circle hitboxes where relevant (Seems to only be explosions, spike ball, roulette wheel, and boulder)

## [0.0.19] - 2023-01-09

### Added

- Bombs to Detection Boxes
- Option to show origin point on hitboxes in the Debug menu
- Ability to link entity values from Raw Entity data as overlay on entity

## [0.0.18] - 2022-12-28

### Added

- Detection Boxes setting to Debug tab that shows various boxes related to influencing enemy AI

## [0.0.17] - 2022-12-23

### Added

- Insertion Point under Debug -> Global State
- Lockable level flags on the Levels menu

## [0.0.16] - 2022-11-07

### Fixed

- Bug related to key modifiers not being registered properly

## [0.0.15] - 2022-11-07

### Added

- Camera State
- Attach Camera to Selected Entity
- Added ability to skip Olmec Cutscene
- Added ability to copy entity bytes to clipboard

## [0.0.14] - 2022-11-05

### Added

- Disable Olmec Gaps

## [0.0.13] - 2022-11-03

### Added

- Landmine to Entity List
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
