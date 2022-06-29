**Specs HD** is a tool for doing science, practice, and fun in
Spelunky HD. In order to use Specs HD just click the Launch button above
after you've started Spelunky HD. We recommend you run `Latest`
unless you know what you're doing. Once you've launched with
**Specs HD** you'll need to restart the game to unload it.

### Key Bindings

The following are the default key bindings. Instructions on how to override bindings will be provided below

#### Keyboard Shortcuts

- `Insert` - Toggle visibility of the main tool window.
- `Ctrl` + `Space` - Pause Game Engine
- `PageDown` - Advance Frame (Can be held for repeated advancement)

#### Mouse Controls

- `Left Click` - Spawn Entities (when enabled)
- `Right Click` - Teleport your spelunker
- `Middle Click` - Select Entity

#### Override Bindings

After you've launched Specs at least once there will be a `specs-hd.toml` file in your Spelunky
installation directory. For both keys and mouse bindings you'll see an array of 3 elements such
as `[1,1,524]`. These elements represent `[FunctionID, KeyModifiers, Key/Mouse]`. The `FunctionID`
should never be changed as Specs expects all of them to exist.

##### Key Modifiers

`KeyModifiers` is a bit mask of which modifier keys should be held with `0` meaning none. The values for different modifier keys are as follows:

```
None  =
Ctrl  = 1
Shift = 2
Alt   = 4
Super = 8
```

These values and be bitwise or'd together to combine multiple modifier keys. For example `Ctrl + Shift` would be the value `3`

##### Keys

`Keys` are an ID that map to a specific key on the keyboard. The list of available keys and their IDs is available at https://github.com/spelunky-fyi/SpecsHD/blob/main/keycodes.txt

##### Mouse Buttons

When setting bindings for a mouse the following options are available:

```
Left Button   = 0
Right Button  = 1
Middle Button = 2
Button 4      = 3
Button 5      = 4
```
