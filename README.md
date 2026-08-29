# Skink C++

Skink C++ is the clean native foundation for the Skink desktop drawing application.

## Why this repository exists

The first Skink prototype used a web UI. This repository starts the desktop application again with a native C++ architecture so the canvas, tools, image processing, layers and future brush engine can grow without depending on a browser runtime.

The old web demo is preserved under `reference/` only as a visual/functional reference. It is not part of the runtime.

## Stack

- C++20
- Qt 6 Widgets
- CMake
- Windows-first desktop target

## Current 0.1 foundation

- Native Skink main window
- Native C++ canvas
- Basic pressure-ready brush-engine interface
- Simple brush implementation for testing the canvas
- Floating draggable panels
- Tools, Brush, Layers, Color and Inspector panels
- Panels stay inside the application workspace
- Active panel moves to the front
- Modular folders separating app, core and UI code

## Requirements on Windows

1. Visual Studio 2022 with **Desktop development with C++** installed.
2. Qt 6.5 or newer built for the same MSVC toolchain.
3. CMake 3.24 or newer.

Qt is open source under LGPL/GPL/commercial licensing. If Skink is distributed later, we must deliberately review the Qt licensing/distribution setup before release. For local development this does not block us.

## Configure and build

Open a terminal inside this folder. If Qt is not globally discoverable, replace the example path with your Qt installation:

    cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"
    cmake --build build --config Debug

Then run the generated `Skink.exe` from the build output.

## Architecture

`src/app` owns the desktop application shell.

`src/core` contains application-independent drawing logic. The brush engine is behind `IBrushEngine`, so the temporary brush can later be replaced without rewriting the UI.

`src/ui` contains reusable desktop widgets and floating panels.

`reference` contains the previous web prototype for migration reference only.

## Next recommended phase

Do not add advanced brushes yet. First validate the native window, panel behavior, stylus/mouse input and canvas coordinate system. Then migrate layers and history as independent modules.
