# JDModLoader

## Details
- this library loads all the library dlls, which are contained in the `<Game>/mods/`-folder
- utilizes wrapping mostly unused dinput8.dll functionality in dark souls 3 and elden ring
  - can work in other games, if they only use `DirectInput8Create()` as an imported function from the original dinput8.dll
- mods are loaded after the game calls `DirectInput8Create()` function

## Notes
- this was really hasty written, so do not expect perfect code or results
- mostly used for testing purposes

## Installation
- include the contents of the release archive into the main executions folder (same dir as the exe)
- have in mind, that it loads the libraries without any checks for ready game data, this should be delt with in the mods themselves
