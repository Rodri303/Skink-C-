# SWINK to Skink C++ architecture migration

Branch: `architecture/swink-foundation`

## Goal

Use the mature behavior in `Rodri303/SWINK` as a functional reference while rebuilding the drawing core as native C++ modules. WebView-specific workarounds must not be copied unless the same problem exists in the native input path.

## Integrated in this foundation

### Persistent document model

`DrawingDocument` owns the document bitmap independently from `CanvasWidget`.

This fixes an important limitation of the original C++ prototype: resizing the viewport no longer resizes the drawing bitmap. The document can now evolve independently toward layers, tiles and large-canvas storage.

### Viewport separated from document coordinates

`CanvasWidget` now maps viewport coordinates to document coordinates through a transform. Zoom and pan affect only the view, not the stored pixels.

Current controls:

- Mouse wheel: zoom around cursor.
- Middle-button drag: pan.
- `0`: reset view.

Rotation is intentionally not implemented in this first foundation; it should join the same transform model rather than being added as a special case.

### Stroke processing separated from brush rendering

`StrokeEngine` now owns smoothing, interpolation and spacing between raw input samples and the brush renderer. This ports the useful architectural idea behind SWINK's JavaScript brush processing without copying the browser implementation.

The brush engine remains replaceable through `IBrushEngine`.

### Rich stylus samples

`BrushSample` now preserves:

- position
- pressure
- X/Y tilt
- rotation
- tangential pressure
- timestamp

The temporary brush currently uses pressure only, but future brush engines no longer need a breaking API change to access the remaining stylus data.

### Provisional history module

`HistoryManager` provides isolated undo/redo with a bounded number of `QImage` states. `Ctrl+Z` and redo shortcuts are connected in the canvas.

This deliberately mirrors SWINK's snapshot-based semantics only as a temporary implementation. The module boundary exists so it can later be replaced by tile/delta history without rewriting the canvas or UI.

## Deliberately not migrated

The following SWINK behavior should be treated as reference, not copied directly:

- WebView/Wacom synthetic-event workarounds.
- Browser focus recovery hacks.
- PointerEvent-specific gesture policy.
- HTML Canvas rendering assumptions.
- Data-URL based history snapshots.

Native Qt/Windows input should be validated first. Only reproduce a workaround if the native path demonstrates the same problem.

## Current rendering boundary

The current renderer is still the prototype path:

`StrokeEngine -> IBrushEngine -> QPainter -> QImage`

This is suitable for validating architecture and behavior, but it is not intended to become the final high-performance renderer.

## Recommended next architecture phases

1. Add a real document/layer model above `DrawingDocument`.
2. Add canvas rotation to the existing viewport transform.
3. Move tool state out of `MainWindow`/`CanvasWidget` into a tool controller.
4. Add a native input normalization layer between Qt events and `StrokeEngine`.
5. Port SWINK brush dynamics as data-driven brush settings instead of hard-coded presets.
6. Replace snapshot history with commands, dirty regions or tile deltas before large documents are supported.
7. Define a renderer interface and dirty-region API before selecting the final GPU backend.
8. Benchmark QPainter baseline, then evaluate a Windows-first GPU renderer such as Direct3D 12 without coupling document/brush logic to D3D12.
9. Port process recording at the document-operation/stroke level rather than recording rendered frames.
10. Add serialization only after document/layer/history boundaries stabilize.

## Important design rule

Qt may own windows, widgets and native tablet event delivery. Qt must not become the permanent owner of brush semantics, document semantics or renderer architecture.
