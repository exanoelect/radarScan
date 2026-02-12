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
    #gpiod
)

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
)

# Generate executable
qt_add_executable(radarScan
    ${SOURCES}
    radar.h
    gpio.h gpio.cpp
    volume.h volume.cpp
    brightness.h brightness.cpp


)

# Tambahkan folder tempat qcustomplot berada
target_include_directories(radarScan
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ===== Platform detection =====
if(UNIX AND NOT APPLE)
    message(STATUS "Linux build detected (RPi 5) — linking gpiod")
    find_library(GPIOD_LIBRARY gpiod REQUIRED)
    target_link_libraries(radarScan PRIVATE ${GPIOD_LIBRARY})
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
    #gpiod
)

# macOS bundle settings
if(APPLE)
    set_target_properties(radarScan PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.myqt6app"
        MACOSX_BUNDLE_BUNDLE_NAME "radarScan"
    )
endif()
