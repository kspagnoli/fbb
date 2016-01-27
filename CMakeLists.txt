cmake_minimum_required(VERSION 2.8.11)

project(testproject)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Add boost includes
include_directories(
    $ENV{BOOST_ROOT}
    CMAKE_INCLUDE_CURRENT_DIR)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)

# Source files
set(SOURCE
    DraftDelegate.cpp
    DraftDialog.cpp
    DraftSettings.cpp
    PlayerSortFilterProxyModel.cpp
    PlayerTableModel.cpp
    HittingStats.cpp
    Owner.cpp
    PitchingStats.cpp
    PlayerAppearances.cpp
    PlayerScatterPlotChart.cpp
    SelectedPlayer.cpp
    main.cpp)

file(GLOB HEADERS *.h)

# Tell CMake to create the helloworld executable
add_executable(helloworld WIN32 ${SOURCE} ${HEADERS})

# Use the Widgets module from Qt 5.
target_link_libraries( helloworld Qt5::Widgets)
target_link_libraries( helloworld Qt5::Charts)

