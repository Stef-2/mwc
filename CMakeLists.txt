cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)
include(CMakePrintHelpers)

project(motor LANGUAGES CXX)

# project configuration
set(MTR_DEBUG 1)
set(MTR_LOG 1)

# project directory paths
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/source)
set(SOURCE_STD_DIR ${SOURCE_DIR}/std)
set(MODULE_DIR ${SOURCE_DIR})
set(LOG_DIR ${PROJECT_SOURCE_DIR}/log)

# if logging is enabled, attempt to find the logging directory, if not present, create one
if(${MTR_LOG})
    message(CHECK_START "logging is enabled, attempting to find the log directory...")
    if(EXISTS ${LOG_DIR})
        message(CHECK_PASS "logging directory found: " ${LOG_DIR})
    else()
        message(CHECK_FAIL "logging directory not found, creating one at: " ${LOG_DIR})
        make_directory(${LOG_DIR})
    endif()
endif()

# attempt to find the std module file
set(STD_MODULE_FILE std.cc)
if(NOT EXISTS ${SOURCE_STD_DIR}/${STD_MODULE_FILE})
    message(CHECK_START "attempting to find the std module file...")
    cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH CXX_COMPILER_DIR)
    cmake_path(GET CXX_COMPILER_DIR PARENT_PATH CXX_COMPILER_ROOT)
    file(GLOB_RECURSE STD_MODULE_FILEPATH LIST_DIRECTORIES false ABSOLUTE ${CXX_COMPILER_ROOT}/*/${STD_MODULE_FILE})
    if(STD_MODULE_FILEPATH)
        message(CHECK_PASS "std module file found: " ${STD_MODULE_FILEPATH} " copying std module file to: " ${SOURCE_STD_DIR})
        file(COPY ${STD_MODULE_FILEPATH} DESTINATION ${SOURCE_STD_DIR})
    else()
        message(CHECK_FAIL "std module file not found")
    endif()
endif()

# std module library
add_library(std)
target_sources(std PUBLIC FILE_SET CXX_MODULES FILES ${SOURCE_STD_DIR}/${STD_MODULE_FILE})

# engine executable
add_executable(${PROJECT_NAME})
# engine module sources
target_sources(${PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES
    ${MODULE_DIR}/motor/core/definition/definition.cppm
    ${MODULE_DIR}/motor/core/utility/enum_bitwise_operators.cppm
    ${MODULE_DIR}/motor/core/utility/bit_shift.cppm
    ${MODULE_DIR}/motor/core/string/string.cppm
    ${MODULE_DIR}/motor/core/concept/concept.cppm
    ${MODULE_DIR}/motor/core/output/output_stream.cppm
    ${MODULE_DIR}/motor/core/diagnostic/log/sink.cppm
    ${MODULE_DIR}/motor/core/diagnostic/log/log.cppm
    ${MODULE_DIR}/motor/core/diagnostic/debug.cppm
    ${MODULE_DIR}/motor/core/diagnostic/assert.cppm
)
# engine translation units
target_sources(${PROJECT_NAME} PRIVATE
    ${SOURCE_DIR}/motor/motor.cpp
)
# engine compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE MTR_DEBUG=${MTR_DEBUG} MTR_LOG=${MTR_LOG})
target_link_libraries(${PROJECT_NAME} PUBLIC std stdc++exp)