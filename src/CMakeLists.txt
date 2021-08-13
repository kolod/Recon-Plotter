cmake_minimum_required(VERSION 3.5)

project(application VERSION 0.1 LANGUAGES CXX)

set(PROJECT_NAME "Recon Plotter")
set(PROJECT_DESCRIPTION "Qt-based application for the processing signal recorder 'RECON' data and plotting.")
set(PROJECT_AUTHOR "Oleksandr Kolodkin")
set(PROJECT_URL "https://github.com/kolod/Recon-Plotter")
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(TS_FILES 
	recon-plotter_ru.ts
	recon-plotter_ua.ts
	recon-plotter_de.ts
)

set_source_files_properties(${TS_FILES}
	PROPERTIES OUTPUT_LOCATION
	"${CMAKE_CURRENT_SOURCE_DIR}/i18n"
)

set(QRC_FILES
	icons.qrc
	translations.qrc
)

set(APP_ICON_RESOURCE_WINDOWS "")
if(WIN32)
	set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")
endif()

set(PROJECT_SOURCES
	main.cpp
	utils.h
	utils.cpp
	analogsignal.h
	analogsignal.cpp
	doublelineedit.h
	doublelineedit.cpp
	datafile.h
	datafile.cpp
	recontextfile.h
	recontextfile.cpp
	chartwindow.h
	chartwindow.cpp
	signalsmodel.h
	signalsmodel.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
	${TS_FILES}
	${APP_ICON_RESOURCE_WINDOWS}
)

qt_add_binary_resources(resources ${QRC_FILES})
qt_add_resources(PROJECT_SOURCES ${QRC_FILES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(application MANUAL_FINALIZATION ${PROJECT_SOURCES})
	qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
	if(ANDROID)
		add_library(application SHARED ${PROJECT_SOURCES})
	else()
		add_executable(application ${PROJECT_SOURCES})
	endif()

	qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

add_dependencies(application resources)

target_link_libraries(application PRIVATE 
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Charts
)

set_target_properties(application PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(application)
endif()
