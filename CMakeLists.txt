cmake_minimum_required(VERSION 3.5)

project(recon-plotter VERSION 0.1 LANGUAGES CXX)


enable_testing()
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(RECON_PLOTTER_ENABLE_TESTS OFF)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

if(ANDROID)
	set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
	if (ANDROID_ABI STREQUAL "armeabi-v7a")
		set(ANDROID_EXTRA_LIBS
			${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
			${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so
		)
	endif()
endif()


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Charts OPTIONAL_COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Charts OPTIONAL_COMPONENTS Test)

add_subdirectory(src build/src)

if(RECON_PLOTTER_ENABLE_TESTS)
	add_subdirectory(tests build/tests)
endif()
