cmake_minimum_required(VERSION 2.8.11)

# Project name
project(fbb)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Add boost includes
include_directories(
    $ENV{BOOST_ROOT}
    CMAKE_INCLUDE_CURRENT_DIR
    "include")

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)

# QT resrouces files
qt5_add_resources(RESOURCES 
    Resources.qrc)

# All headers
file(GLOB HEADERS include/FBB/*.h)

# All source
file(GLOB SOURCE source/*.cpp)

# Tell CMake to create the helloworld executable
add_executable(fbb WIN32 main.cpp ${SOURCE} ${HEADERS} ${RESOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries(fbb Qt5::Widgets)
target_link_libraries(fbb Qt5::Charts)

