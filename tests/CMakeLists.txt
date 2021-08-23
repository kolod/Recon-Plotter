cmake_minimum_required(VERSION 3.12)

project(tests LANGUAGES CXX)

set(TEST_001_SOURCES
	../src/utils.h
	../src/utils.cpp
	../src/analogsignal.h
	../src/analogsignal.cpp
	../src/datafile.h
	../src/datafile.cpp
	../src/recontextfile.h
	../src/recontextfile.cpp
	tst_testrecontextfile.cpp
)

set(OTHER_FILES
	test_data.txt
)

configure_file(${CMAKE_SOURCE_DIR}/tests/test_data.txt ${CMAKE_CURRENT_BINARY_DIR}/test_data.txt COPYONLY)
add_compile_definitions(TESTING)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(test_001 MANUAL_FINALIZATION ${TEST_001_SOURCES})
else()
	if(ANDROID)
		add_library(test_001 SHARED ${TEST_001_SOURCES})
	else()
		add_executable(test_001 ${TEST_001_SOURCES})
	endif()
endif()

target_link_libraries(test_001 PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(test_001 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(test_001)
endif()

add_test(NAME test_001 COMMAND test_001)

#################################

set(TEST_002_SOURCES
	../src/utils.h
	../src/utils.cpp
	tst_utils.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(test_002 MANUAL_FINALIZATION ${TEST_002_SOURCES})
else()
	if(ANDROID)
		add_library(test_002 SHARED ${TEST_002_SOURCES})
	else()
		add_executable(test_002 ${TEST_002_SOURCES})
	endif()
endif()

target_link_libraries(test_002 PRIVATE Qt${QT_VERSION_MAJOR}::Test)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(test_002)
endif()

add_test(NAME test_002 COMMAND test_002)
