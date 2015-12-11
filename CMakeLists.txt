cmake_minimum_required(VERSION 2.8.11)

project(testproject)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Add boost includes
include_directories($ENV{BOOST_ROOT})

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

# Source files
set(SOURCE
    main.cpp
    HitterSortFilterProxyModel.cpp
    HitterTableModel.cpp
    PitcherSortFilterProxyModel.cpp
    PitcherTableModel.cpp
    PlayerAppearances.cpp
    SelectedPlayer.cpp)

# Tell CMake to create the helloworld executable
add_executable(helloworld WIN32 ${SOURCE})

foreach(plugin ${Qt5GUI_PLUGINS})
  get_target_property(_loc ${plugin} LOCATION)
  message("Plugin ${plugin} is at location ${_loc}")
endforeach()

# Use the Widgets module from Qt 5.
target_link_libraries(helloworld Qt5::Widgets)
