cmake_minimum_required(VERSION 3.5)

project(application VERSION 0.1 LANGUAGES CXX)

set(TS_FILES 
	recon-plotter_ru.ts
	recon-plotter_ua.ts
)
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

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
	qt_add_executable(application
		MANUAL_FINALIZATION
		${PROJECT_SOURCES}
	)

	qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
	if(ANDROID)
		add_library(application SHARED
			${PROJECT_SOURCES}
		)
	else()
		add_executable(application
			${PROJECT_SOURCES}
		)
	endif()

	qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

add_dependencies(application resources)

target_link_libraries(application PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(application PRIVATE Qt${QT_VERSION_MAJOR}::Charts)

set_target_properties(application PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(application)
endif()
