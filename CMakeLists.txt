cmake_minimum_required(VERSION 3.2)


set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.12.2\\5.12.2\\msvc2017_64")

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
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Charts CONFIG REQUIRED)

# QT resrouces files
qt5_add_resources(RESOURCES 
    Resources.qrc)

# All headers
set(MOC_HEADERS
	include/FBB/FBBApplication.h
	include/FBB/FBBDraftBoard.h
	include/FBB/FBBDraftBoardModel.h
	include/FBB/FBBDraftBoardSortFilterProxyModel.h
	include/FBB/FBBDraftDialog.h
	include/FBB/FBBDraftLog.h
	include/FBB/FBBDraftLogModel.h
	include/FBB/FBBLeaugeSettings.h
	include/FBB/FBBLeaugeSettingsDialog.h
	include/FBB/FBBMainMenuBar.h
	include/FBB/FBBMainWindow.h
	include/FBB/FBBPlayer.h
	include/FBB/FBBPosition.h
	include/FBB/FBBProjectionService.h
	include/FBB/FBBTeam.h)

# All source
set(SOURCE 
	source/FBBApplication.cpp
	source/FBBDraftBoard.cpp
	source/FBBDraftBoardModel.cpp
	source/FBBDraftBoardSortFilterProxyModel.cpp
	source/FBBDraftDialog.cpp
	source/FBBDraftLog.cpp
	source/FBBDraftLogModel.cpp
	source/FBBLeaugeSettings.cpp
	source/FBBLeaugeSettingsDialog.cpp
	source/FBBMainMenuBar.cpp
	source/FBBMainWindow.cpp
	source/FBBPosition.cpp
	source/FBBPlayer.cpp
	source/FBBProjectionService.cpp
	source/FBBTeam.cpp)

# Tell CMake to create the helloworld executable
add_executable(fbb WIN32 main.cpp ${SOURCE} ${MOC_HEADERS} ${RESOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries(fbb Qt5::Widgets)
target_link_libraries(fbb Qt5::Charts)


# JSON TOOL
add_executable(fbb_json WIN32 fbb_json.cpp ${RESOURCES})
target_link_libraries(fbb_json Qt5::Core)
target_link_libraries(fbb_json Qt5::Widgets)
set_target_properties(fbb_json PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
