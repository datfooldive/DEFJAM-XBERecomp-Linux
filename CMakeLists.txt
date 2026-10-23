# Def Jam: Fight for NY (original Xbox), statically recompiled

cmake_minimum_required(VERSION 3.25)
project(defjam LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# the SDK: sdk/ here, a checkout beside this repository, else downloaded
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/sdk/cmake/XbeRecomp.cmake)
    set(SDK ${CMAKE_CURRENT_SOURCE_DIR}/sdk)
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../XboxXBE-Recomp/cmake/XbeRecomp.cmake)
    set(SDK ${CMAKE_CURRENT_SOURCE_DIR}/../XboxXBE-Recomp)
else()
    include(FetchContent)
    FetchContent_Declare(xbe_sdk GIT_REPOSITORY https://github.com/TGP482/XboxXBE-Recomp.git GIT_TAG main)
    FetchContent_MakeAvailable(xbe_sdk)
    set(SDK ${xbe_sdk_SOURCE_DIR})
endif()
include(${SDK}/cmake/XbeRecomp.cmake)
# src/common/game_files.cpp replaces the SDK's, which is Windows' (windows.h, MessageBoxW)
list(FILTER XBE_RUNTIME_SOURCES EXCLUDE REGEX "/app/game_files\\.cpp$")
# src/d3d/render_thread.cpp is the same file with the Windows thread naming taken out
list(FILTER XBE_RUNTIME_SOURCES EXCLUDE REGEX "/gpu/render_thread\\.cpp$")

file(GLOB XBE_HOST_SOURCES CONFIGURE_DEPENDS src/*/*.cpp)
list(FILTER XBE_HOST_SOURCES EXCLUDE REGEX "/src/game/")

# partitions as xbe_symbols wrote them into config/game.toml
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS config/game.toml)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config/game.toml)
    file(STRINGS config/game.toml GAME_PARTITIONS REGEX "^partitions *=")
    string(REGEX MATCH "[0-9]+" GAME_PARTITIONS "${GAME_PARTITIONS}")
endif()
# no config yet: build.sh writes it with xbe_symbols, the change reconfigures
if(NOT GAME_PARTITIONS)
    set(GAME_PARTITIONS 64)
endif()

# target name = GAME_EXE in build.sh, xbe = GAME_XBE
add_xbe_image(DefJam default.xbe game.toml ${GAME_PARTITIONS} game)

if(WIN32)
    set_target_properties(DefJam PROPERTIES OUTPUT_NAME DefJam)
else()
    set_target_properties(DefJam PROPERTIES OUTPUT_NAME defjam)
    # The SDK ships no Direct3D 12 or XAudio2 for Linux: gpu::CreateBackend comes from
    # src/d3d/vulkan_linux.cpp and audio::StartOutput from src/audio/sdl_linux.cpp, both
    # picked up by the src/*/*.cpp glob.
endif()
