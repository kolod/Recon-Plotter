cmake_minimum_required(VERSION 3.5)

project(tests LANGUAGES CXX)


set(TEST_001_SOURCES
    ../src/utils.h
	../src/utils.cpp
	../src/analog_signal.h
	../src/analog_signal.cpp
	../src/data_file.h
	../src/data_file.cpp
	../src/recon_text_file.h
	../src/recon_text_file.cpp
	tst_testrecontextfile.cpp
	test_data.txt
)

configure_file(${CMAKE_SOURCE_DIR}/tests/test_data.txt ${CMAKE_CURRENT_BINARY_DIR}/test_data.txt COPYONLY)
add_compile_definitions(TESTING)
add_executable(test_001 ${TEST_001_SOURCES})
add_test(NAME test_001 COMMAND test_001)

target_link_libraries(test_001 PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(test_001 PRIVATE Qt${QT_VERSION_MAJOR}::Charts)

##################################

set(TEST_002_SOURCES
    ../src/utils.h
	../src/utils.cpp
	tst_utils.cpp
)

add_executable(test_002 ${TEST_002_SOURCES})
add_test(NAME test_002 COMMAND test_002)

target_link_libraries(test_002 PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(test_002 PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
