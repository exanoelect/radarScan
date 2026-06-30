cmake_minimum_required(VERSION 3.16)

project(radarScan VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Temukan modul Qt6 yang diperlukan
find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    SerialPort
    Network
    Multimedia
    WebSockets
    Core
    DBus
    Mqtt
)

# libnl (netlink)
find_package(PkgConfig REQUIRED)

if(UNIX AND NOT APPLE)
    pkg_check_modules(LIBNL REQUIRED libnl-3.0 libnl-genl-3.0)
endif()

# Jika pakai .ui file
qt_standard_project_setup()

# Kumpulkan file sumber
set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    qcustomplot.cpp
    qcustomplot.h
    socketioclient.cpp
    socketioclient.h
    socketeventworker.h
    socketeventworker.cpp
    payloadprocessor.h
    payloadprocessor.cpp
    configmanager.h
    configmanager.cpp
    audioworker.h
    audioworker.cpp
    networksetting.h
    networksetting.cpp
    utilities.h
    utilities.cpp
    systemdmonitorqt.h
    systemdmonitorqt.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND SOURCES
        VolumeMonitor.h
        VolumeMonitor.cpp
        NetworkMonitorQt.h
        NetworkMonitorQt.cpp
        networkmonitor.h
        networkmonitor.cpp
    )
endif()

# Generate executable
qt_add_executable(radarScan
    ${SOURCES}
    radar.h
    gpio.h gpio.cpp
    volume.h volume.cpp
    brightness.h brightness.cpp
)

# Include path
target_include_directories(radarScan
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBNL_INCLUDE_DIRS}
)

# ===== Platform detection =====
if(UNIX AND NOT APPLE)
    message(STATUS "Linux build detected (RPi 5) — linking gpiod + libnl")

    find_library(GPIOD_LIBRARY gpiod REQUIRED)

    target_link_libraries(radarScan PRIVATE
        ${GPIOD_LIBRARY}
        ${LIBNL_LIBRARIES}
    )

    target_compile_definitions(radarScan PRIVATE PLATFORM_LINUX)

elseif(APPLE)
    message(STATUS "macOS build detected — gpiod stub mode")

    target_compile_definitions(radarScan PRIVATE PLATFORM_MACOS)
endif()

# Link Qt modules (common)
target_link_libraries(radarScan PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Network
    Qt6::Multimedia
    Qt6::WebSockets
    Qt6::Core
    Qt6::DBus
    Qt6::Mqtt
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(radarScan PRIVATE pulse)
endif()

# Tambahan compile flags dari pkg-config
target_compile_options(radarScan PRIVATE
    ${LIBNL_CFLAGS_OTHER}
)

# macOS bundle settings
if(APPLE)
    set_target_properties(radarScan PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.myqt6app"
        MACOSX_BUNDLE_BUNDLE_NAME "radarScan"
    )
endif()
