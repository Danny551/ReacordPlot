cmake_minimum_required(VERSION 3.5)

project(plot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

set(src_
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    ./qcustomplot/qcustomplot.cpp)

set(inc_
    mainwindow.h
    ./qcustomplot/qcustomplot.h)

add_executable(plot ${src_} ${inc_})

target_link_libraries(plot PRIVATE Qt5::Widgets Qt5::PrintSupport)


