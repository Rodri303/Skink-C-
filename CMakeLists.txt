cmake_minimum_required(VERSION 3.24)

project(Skink VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Gui)

qt_standard_project_setup()

qt_add_executable(Skink
    src/main.cpp
    src/app/MainWindow.cpp
    src/app/MainWindow.hpp
    src/core/brush/IBrushEngine.hpp
    src/core/brush/BasicBrushEngine.cpp
    src/core/brush/BasicBrushEngine.hpp
    src/core/canvas/CanvasWidget.cpp
    src/core/canvas/CanvasWidget.hpp
    src/core/document/DrawingDocument.cpp
    src/core/document/DrawingDocument.hpp
    src/core/history/HistoryManager.cpp
    src/core/history/HistoryManager.hpp
    src/core/stroke/StrokeEngine.cpp
    src/core/stroke/StrokeEngine.hpp
    src/core/tools/ToolController.cpp
    src/core/tools/ToolController.hpp
    src/ui/input/ShortcutRouter.cpp
    src/ui/input/ShortcutRouter.hpp
    src/ui/panels/DraggablePanel.cpp
    src/ui/panels/DraggablePanel.hpp
    src/ui/widgets/ToolButton.cpp
    src/ui/widgets/ToolButton.hpp
    src/ui/workspace/WorkspaceWidget.cpp
    src/ui/workspace/WorkspaceWidget.hpp
    src/ui/workspace/WorkspacePersistence.cpp
    src/ui/workspace/WorkspacePersistence.hpp
    src/ui/topbar/TopBar.cpp
    src/ui/topbar/TopBar.hpp
    src/ui/toolrail/ToolRail.cpp
    src/ui/toolrail/ToolRail.hpp
    src/ui/brush/BrushControls.cpp
    src/ui/brush/BrushControls.hpp
    src/ui/brush/QuickBrushPanel.cpp
    src/ui/brush/QuickBrushPanel.hpp
    src/ui/layers/LayersPanel.cpp
    src/ui/layers/LayersPanel.hpp
    src/ui/docking/SkinkDockPanel.cpp
    src/ui/docking/SkinkDockPanel.hpp
    src/ui/bottom/BottomDock.cpp
    src/ui/bottom/BottomDock.hpp
    src/ui/hud/CanvasHud.cpp
    src/ui/hud/CanvasHud.hpp
    src/ui/color/ColorPicker.cpp
    src/ui/color/ColorPicker.hpp
    src/ui/color/ColorPanel.cpp
    src/ui/color/ColorPanel.hpp
    src/ui/color/ColorWheel.cpp
    src/ui/color/ColorWheel.hpp
)

target_include_directories(Skink PRIVATE src)

target_link_libraries(Skink PRIVATE Qt6::Widgets Qt6::Gui)

if(WIN32)
    set_target_properties(Skink PROPERTIES WIN32_EXECUTABLE TRUE)

    add_custom_target(SkinkDeployDebug
        COMMAND $<TARGET_FILE:Qt6::windeployqt> --debug --compiler-runtime $<TARGET_FILE:Skink>
        DEPENDS Skink
        COMMENT "Deploying the Qt Debug runtime for Skink"
        VERBATIM
    )
endif()
