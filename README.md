# SpyGameQt

`SpyGameQt` is the Qt Widgets desktop version of the Spy Game project.

## Included Files

- `Spy_game.pro`: qmake project file
- `main.cpp`: application entry point
- `mainwindow.h` / `mainwindow.cpp`: game window logic
- `mainwindow.ui`: Qt Designer interface
- `resources.qrc`: embedded app resources
- `words.txt`: bundled word list used by the game

## Build Requirements

- Qt 6 Widgets
- `qmake`
- `make`
- macOS command line tools when building on macOS

## Build And Run

From the `Spy_game` folder:

```bash
mkdir -p build
cd build
qmake ../Spy_game.pro CONFIG+=release
make
open SpyGameQt.app
```

If your Qt tools are not in `PATH`, run `qmake` and `macdeployqt` from your local Qt installation instead of using a hard-coded absolute path.

## Create A Self-Contained macOS App

After building:

```bash
macdeployqt SpyGameQt.app
```

## Notes

- The word list is embedded through `resources.qrc`, so the app does not depend on an external `words.txt` at runtime.
- Saved user and result files are written to the standard app-data location for `SpyGameQt`.
- The generated `.app` bundle is not committed to the repository and should be built locally.
