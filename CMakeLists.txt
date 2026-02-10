cmake_minimum_required(VERSION 3.25.0)

project(vstgui-wine-test-plugin
    VERSION 1.0.0.1
    DESCRIPTION "VSTGUI D2D1 Wine Test Plugin"
)

set(vst3sdk_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vst3sdk")

# Cross-compilation: set output path and disable symlink creation to avoid
# Windows admin scripts that fail on Linux
set(SMTG_PLUGIN_TARGET_USER_PATH "${CMAKE_BINARY_DIR}/VST3" CACHE PATH "" FORCE)
set(SMTG_CREATE_PLUGIN_LINK OFF CACHE BOOL "" FORCE)

# Disable examples and hosting to speed up the build
# VSTGUI support ON — SDK's vstgui4/ is symlinked to our patched fork
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMTG_RUN_VST_VALIDATOR OFF CACHE BOOL "" FORCE)
set(SMTG_ENABLE_VSTGUI_SUPPORT ON CACHE BOOL "" FORCE)
set(SMTG_CREATE_BUNDLE_FOR_WINDOWS ON CACHE BOOL "" FORCE)

# VSTGUI standalone not needed for plugin and fails with MinGW cross-compilation
set(VSTGUI_STANDALONE OFF CACHE BOOL "" FORCE)

# Disable UI scripting — we use programmatic UI, and TinyJS fails with MinGW
# (missing <cstdint> include for int64_t)
set(VSTGUI_UISCRIPTING OFF CACHE BOOL "" FORCE)

add_subdirectory(${vst3sdk_SOURCE_DIR} ${CMAKE_BINARY_DIR}/vst3sdk)

set(plugin_sources
    source/processor.h
    source/processor.cpp
    source/controller.h
    source/controller.cpp
    source/editor.h
    source/editor.cpp
    source/waveformviews.h
    source/pluginentry.cpp
    source/plugincids.h
    source/pluginparamids.h
    source/version.h
)

# Required for smtg_target_add_library_main to find dllmain.cpp
set(public_sdk_SOURCE_DIR "${vst3sdk_SOURCE_DIR}/public.sdk")

smtg_add_vst3plugin(d2d1-wine-test ${plugin_sources})

target_compile_features(d2d1-wine-test PUBLIC cxx_std_17)

target_link_libraries(d2d1-wine-test
    PRIVATE
        sdk
        vstgui_support
        sdk       # repeated: vstgui_support needs symbols from sdk (MinGW link order)
        vstgui
        vstgui_uidescription
)

if(SMTG_WIN)
    target_link_libraries(d2d1-wine-test
        PRIVATE
            d2d1 dwrite d3d11 dxgi dcomp dwmapi
            shlwapi imm32 opengl32
    )
endif()
