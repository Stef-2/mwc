cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)
include(CMakePrintHelpers)

project(motor LANGUAGES CXX)

# project directory paths
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/source)
set(SOURCE_STD_DIR ${SOURCE_DIR}/std)
set(MODULE_DIR ${SOURCE_DIR})

# attempt to find the std module file
message(CHECK_START "attempting to find the std module file...")
cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH CXX_COMPILER_DIR)
cmake_path(GET CXX_COMPILER_DIR PARENT_PATH CXX_COMPILER_ROOT)
set(STD_MODULE_FILE std.cc)
file(GLOB_RECURSE STD_MODULE_FILEPATH LIST_DIRECTORIES false ABSOLUTE ${CXX_COMPILER_ROOT}/*/${STD_MODULE_FILE})
if(STD_MODULE_FILEPATH)
    message(CHECK_PASS "found std module file: " ${STD_MODULE_FILEPATH} " copying std module file to: " ${SOURCE_STD_DIR})
    file(COPY ${STD_MODULE_FILEPATH} DESTINATION ${SOURCE_STD_DIR})
else()
    message(CHECK_FAIL "could not find std module file")
endif()

# std module library
add_library(std)
target_sources(std PUBLIC FILE_SET CXX_MODULES FILES ${SOURCE_STD_DIR}/${STD_MODULE_FILE})

# engine executable
add_executable(${PROJECT_NAME})
# engine module sources
target_sources(${PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES
    ${MODULE_DIR}/motor/core/string/string.cppm
)
# engine translation units
target_sources(${PROJECT_NAME} PRIVATE
    ${SOURCE_DIR}/motor/motor.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC std stdc++exp)