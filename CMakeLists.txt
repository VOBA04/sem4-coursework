cmake_minimum_required(VERSION 3.5)

project(Overseer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/vova/Qt/6.7.0/gcc_64")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Cpu.cpp
        Cpu.h
        Memory.cpp
        Memory.h
        Disk.cpp
        Disk.h
        Net.cpp
        Net.h
        qcustomplot.cpp
        qcustomplot.h
        customplot.h
        squarelegenditem.h
        color.h
)

qt_add_executable(Overseer
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    resources.qrc
)

target_link_libraries(Overseer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Overseer PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)

include(GNUInstallDirs)
install(TARGETS Overseer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Overseer)
endif()
