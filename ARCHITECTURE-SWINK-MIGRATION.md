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

## Phase 08.3 - Stroke Entry / Pending Stroke (documented only)

Phase 08.3 must add a pending-stroke input state for contacts that begin inside
the drawing workspace but outside the physical white document. It is explicitly
not implemented in Phase 08.2.

Required future behavior:

- A tip or mouse press outside the document but inside the drawing viewport
  enters `PendingStroke`.
- Crossing into the document starts the stroke without requiring another press.
- The first rendered point is the first valid document entry; no line may be
  drawn from the exterior position to that entry.
- Pressure, X/Y tilt, rotation, tangential pressure and timestamp must be
  retained while pending.
- Brush and Eraser must share the same entry behavior, with a coherent mouse
  equivalent.
- Releasing before entry, losing application focus, or beginning Pan, Rotate or
  Zoom cancels `PendingStroke` without an Undo entry.
- Space/Pan Tool, Ctrl/Rotate, Alt/Zoom and middle-button Pan keep navigation
  priority.
- UI panels and controls must never initiate `PendingStroke`; its valid domain
  is the workspace drawing viewport.
- Entry detection belongs to input/stroke handling, not the brush engine.
- Exact geometric intersection with the document edge can be investigated for
  maximum continuity after the initial behavior is working.

Planned direction:

`Workspace/Input -> PendingStroke state -> StrokeEngine -> BrushEngine`

## Future module: Skink Artwork Reconstruction / Artwork Scanner

This is a future product and research line. It is not part of Phase 08 and must not interrupt Brush Core work or renumber the current phases.

### Problem and product goal

Artwork Reconstruction should let an artist photograph different, partially overlapping regions of a predominantly flat physical artwork and reconstruct one very-high-resolution master document. Target material includes drawings, illustrations, paintings, pages and other flat artistic surfaces.

A grid of 6, 9, 12, 20 or more photographs may record substantially more real surface detail than one full-frame photograph. The goal is to recover detail actually captured by the camera, not merely invent pixels through generic upscaling or AI.

The intended result is a normal Skink document. Artwork Scanner must not introduce a second, incompatible document model.

### Laboratory-first development

Development should begin as an independent Artwork Reconstruction application/prototype rather than inside the main Skink executable. The laboratory should make it possible to measure and iterate on:

- feature detection and matching accuracy;
- incorrect-match rejection;
- homographies, perspective correction and warping quality;
- seam quality and blending;
- exposure, illumination and color consistency;
- stacking and multi-frame reconstruction;
- RAM use and processing time;
- very large reconstructions and large photograph sets.

The prototype protects editor stability, but its Core must not be disposable. Core components should be designed for later integration through explicit Skink interfaces and without depending directly on Qt Widgets or a specific renderer.

External projects are research material. Skink should own its architecture and code while consuming verified libraries through adapters:

`External projects -> research -> Skink-owned architecture/code -> verified APIs/libraries`

### Conceptual architecture

The target direction is approximately:

`Artwork Scanner UI`
`-> ArtworkReconstructionController`
`-> Artwork Reconstruction Core`
`-> computer-vision adapters`
`-> OpenCV or another verified implementation library`
`-> Skink Document / Tile APIs`

Possible Core responsibilities include:

- `ImageAnalysis`;
- `FeatureDetection`;
- `ImageMatching`;
- `GeometryAlignment`;
- `PerspectiveCorrection`;
- `ExposureCompensation`;
- `ColorNormalization`;
- `SeamDetection`;
- `Blending`;
- `ImageStacking`;
- `MultiFrameReconstruction`.

These names are conceptual. Do not create these classes until implementation work actually begins.

### Initial reconstruction pipeline

The initial technical direction is:

1. Import photographs and read available metadata.
2. Identify related photographs and candidate regions.
3. Detect suitable image features.
4. Match features and reject invalid correspondences.
5. Use RANSAC and estimate homographies.
6. Correct perspective and, when appropriate, lens distortion.
7. Warp and align images, including later subpixel refinement.
8. Compensate exposure and normalize photometric/color differences.
9. Detect seams and blend regions.
10. Produce the final reconstruction and master Skink document.

Because the target works are mostly planar, homography estimation, plane geometry, perspective rectification and lens-distortion correction require particular care. These techniques solve related but different problems and must not be treated as interchangeable.

### Stitching, stacking and multi-frame reconstruction

These are separate operations:

- **Stitching/mosaicing** joins photographs of different physical regions. A 3x3 capture grid can reconstruct one larger original.
- **Stacking** combines multiple photographs of the same region to reduce noise, improve signal stability and reduce defects from an individual capture.
- **Multi-frame reconstruction/super-resolution research** investigates whether small capture offsets contain complementary subpixel information that can recover additional real detail. It must not be presented as ordinary upscaling.

An advanced session could contain 9 regions with 3 captures per region (27 images). The system should eventually group A1/A2/A3, B1/B2/B3 and so on; align and combine each same-region group first; then stitch the resulting regions into the full work.

### Color and capture consistency

Geometric alignment alone is insufficient for artwork reproduction. Future research must address:

- exposure and white balance;
- uneven illumination and vignetting;
- color differences between photographs;
- photometric consistency across all regions;
- optional color-chart, neutral-gray and camera calibration workflows;
- capture profiles shared by an entire photographic session.

This must be able to evolve toward Skink's professional color-management architecture. Do not permanently constrain reconstruction data to 8-bit sRGB if the future Color Engine and document format support greater precision or wider color spaces.

### OpenCV and research references

OpenCV is the leading implementation candidate for feature detection, SIFT or other appropriate detectors, matching, homographies, RANSAC, warping, stitching, blending, transformations and photometric image processing. It should be used as an algorithms/operations library behind Skink-owned adapters, not as application architecture.

Do not add OpenCV to the current project. Select and verify a concrete version only when development starts.

Research should include:

- small educational implementations of SIFT -> KNN matching -> RANSAC -> homography -> warping -> blending;
- educational material covering perspective, DLT/RANSAC and stitching stages;
- advanced conceptual references for control points, optimization, photometric correction and professional blending pipelines;
- independently implemented panorama/stitching projects used to understand algorithms and compare architectures.

Research code must not be copied without verifying provenance and license. GPL implementations may be studied conceptually but must not be incorporated into proprietary Skink code without a specific compatibility review.

### Integration prerequisites and boundaries

Main-editor integration depends principally on mature:

- Document and Layer Engine boundaries;
- tiled image storage and processing;
- file/project and resource handling;
- sufficient color architecture;
- stable internal APIs.

Reconstructions made from many photographs may be extremely large. The Tile Engine is therefore a key dependency: processing and storage should support progressive, regional operation instead of assuming one permanently resident monolithic bitmap.

File/project design must later decide whether source photographs are linked, embedded, stored as project resources or discarded after master generation. This decision is intentionally deferred.

Artwork Reconstruction and Canvas Recorder are independent modules. Neither may depend directly on the other, although both may use shared Document, Tiles, Color, storage and regional-processing infrastructure.

The future user flow may resemble `File -> Import -> Artwork Scan`, but its UI is intentionally undefined at this stage.

### Staged future work

1. Build an independent laboratory that loads photograph sets and records accuracy, matching failures, seam quality, distortion, memory use, processing time and practical maximum size.
2. Implement multi-region matching, homography, perspective correction, warping and blending.
3. Add exposure, illumination, vignetting and color-consistency processing.
4. Add same-region classification, alignment and stacking.
5. Research subpixel alignment and genuine multi-frame detail recovery.
6. Integrate the stable pipeline through Skink-owned interfaces.
7. Convert reconstruction output into the normal Skink Document/Tile Engine.
8. Design the final guided user workflow only after the pipeline is reliable.

The independent laboratory may begin earlier than these integration prerequisites because it must not compromise the main Skink codebase.

## External dependency and licensing policy

Every external dependency that may be incorporated into Skink must have a maintained record containing:

- name and exact version;
- official repository/site;
- license;
- copyright holders/authors;
- reason for use;
- Skink components that use it;
- attribution obligations;
- LICENSE/NOTICE files that must be distributed;
- relevant known patent considerations;
- date on which the information was verified.

This information must be checked again against official sources when a dependency is integrated. Historical roadmap notes are not sufficient evidence.

A compatible open-source license is not a complete legal or technical risk assessment. Important algorithms and dependencies require review of license, version, provenance, relevant known patents and distribution obligations. The roadmap must not claim definitive legal conclusions.

The product should eventually provide `Help -> Third-party licenses`. Ideally the UI and distributed third-party notice file should be generated from one dependency register maintained with the project, avoiding duplicated manual records.

## Mandatory pre-release task (Phase 15/16)

Wacom diagnostics cleanup is **mandatory before release**:

- Review every temporary log with the `[WACOM-DIAG]` prefix.
- Remove diagnostics that are no longer necessary.
- If diagnostics remain useful for support or hardware investigation, disable them by default behind an explicit diagnostic mode or flag.
- Verify that a normal Skink build does not produce tablet/input event spam in the console.
- Verify that distribution builds do not create unnecessary Wacom log files.
- Keep a controlled way to reactivate Wacom diagnostics for future tablet and hardware investigations.

This belongs to final optimization/release work. It must not change the current Phase 07.3 diagnostic behavior prematurely.

## Important design rule

Qt may own windows, widgets and native tablet event delivery. Qt must not become the permanent owner of brush semantics, document semantics or renderer architecture.
