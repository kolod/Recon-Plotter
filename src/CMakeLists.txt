cmake_minimum_required(VERSION 3.12)

project(application VERSION 0.1 LANGUAGES CXX)

set(TS_FILES
	resources/i18n/recon-plotter_ru.ts
	resources/i18n/recon-plotter_ua.ts
	resources/i18n/recon-plotter_de.ts
)

set_source_files_properties(${TS_FILES}
	PROPERTIES OUTPUT_LOCATION
	"${CMAKE_CURRENT_SOURCE_DIR}/i18n"
)

set(QRC_FILES
	resources/resources.qrc
	resources/translations.qrc
)

if(NOT WIN32)
	list(APPEND QRC_FILES resources/linuxresources.qrc)
endif()

set(PROJECT_SOURCES
	main.cpp
	utils.h
	utils.cpp
	localsocket.h
	localsocket.cpp
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
        colordelegate.h
        colordelegate.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
	qcustomplot.h
	qcustomplot.cpp
)

if(WIN32)
	list(APPEND PROJECT_SOURCES resources/resources.rc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_binary_resources(resources ${QRC_FILES})
	qt_add_resources(PROJECT_SOURCES ${QRC_FILES})
	qt_add_executable(application MANUAL_FINALIZATION ${PROJECT_SOURCES})
	qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
	qt5_add_binary_resources(resources ${QRC_FILES})
	qt5_add_resources(PROJECT_SOURCES ${QRC_FILES})
	if(ANDROID)
		add_library(application SHARED ${PROJECT_SOURCES})
	else()
		add_executable(application ${PROJECT_SOURCES})
	endif()
	qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

add_dependencies(application resources)

target_link_libraries(application PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(application PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(application PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)

set_target_properties(application PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(application)
endif()
