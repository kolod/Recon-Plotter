cmake_minimum_required(VERSION 3.12)

project(recon-plotter VERSION 0.1 LANGUAGES CXX)

set(PROJECT_NAME "Recon Plotter")
set(PROJECT_DESCRIPTION "Qt-based application for the processing signal recorder 'RECON' data and plotting.")
set(PROJECT_AUTHOR "Oleksandr Kolodkin")
set(PROJECT_URL "https://github.com/kolod/Recon-Plotter")
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CODE_COVERAGE "Enable coverage reporting" ON)
option(BUILD_TESTING "Build the testing tree" ON)
option(QCUSTOMPLOT_DEBUG_ELEMENTS_RECT "Debug code to draw all layout element rects" OFF)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

if(ANDROID)
	set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
	if (ANDROID_ABI STREQUAL "armeabi-v7a")
		set(ANDROID_EXTRA_LIBS
			${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
			${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
	endif()
endif()

find_package(QT NAMES Qt5 Qt6 REQUIRED
		COMPONENTS Widgets Network PrintSupport LinguistTools Gui
	OPTIONAL_COMPONENTS Test
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
		COMPONENTS Widgets Network PrintSupport LinguistTools Gui
	OPTIONAL_COMPONENTS Test
)

set(TARGET_NAME application_qcppainter)
add_subdirectory(src ${CMAKE_BINARY_DIR}/QCPPainter)

set(TARGET_NAME application_qpainter)
set(USE_QPLOTTER ON)
add_subdirectory(src ${CMAKE_BINARY_DIR}/QPainter)

if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
