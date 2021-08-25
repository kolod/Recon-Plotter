cmake_minimum_required(VERSION 3.12)

project(${TARGET_NAME} VERSION 0.1 LANGUAGES CXX)

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

if(USE_QPLOTTER)
	message(STATUS "Use QPlotter")
	set(QCUSTOMPLOT_SOURCES
		../qcustomplot/qplotter/qcustomplot.h
		../qcustomplot/qplotter/qcustomplot.cpp
	)
else()
	message(STATUS "Use QCPPlotter")
	set(QCUSTOMPLOT_SOURCES
		../qcustomplot/qcpplotter/qcustomplot.h
		../qcustomplot/qcpplotter/qcustomplot.cpp
	)
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
	settingsdialog.h
	settingsdialog.cpp
	settingsdialog.ui
	${QCUSTOMPLOT_SOURCES}
)

if(WIN32)
	list(APPEND PROJECT_SOURCES resources/resources.rc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_binary_resources(${TARGET_NAME}_resources ${QRC_FILES})
	qt_add_resources(PROJECT_SOURCES ${QRC_FILES})
	qt_add_executable(${TARGET_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
	qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
	qt5_add_binary_resources(${TARGET_NAME}_resources ${QRC_FILES})
	qt5_add_resources(PROJECT_SOURCES ${QRC_FILES})
	if(ANDROID)
		add_library(${TARGET_NAME} SHARED ${PROJECT_SOURCES})
	else()
		add_executable(${TARGET_NAME} ${PROJECT_SOURCES})
	endif()
	qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

if(USE_QPLOTTER)
	target_include_directories(${TARGET_NAME} BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/qcustomplot/qplotter)
	target_compile_definitions(${TARGET_NAME} PUBLIC "QCPPainter=QPainter")
else()
	target_include_directories(${TARGET_NAME} BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/qcustomplot/qcpplotter)
endif()

if(QCUSTOMPLOT_DEBUG_ELEMENTS_RECT)
	target_compile_definitions(${TARGET_NAME} PUBLIC QCUSTOMPLOT_DEBUG_ELEMENTS_RECT)
endif()

add_dependencies(${TARGET_NAME} ${TARGET_NAME}_resources)

target_link_libraries(${TARGET_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${TARGET_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(${TARGET_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)

set_target_properties(${TARGET_NAME} PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(${TARGET_NAME})
endif()
