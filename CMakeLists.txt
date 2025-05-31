cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)
#include(CMakePrintHelpers)

project(mwc LANGUAGES CXX)

# project configuration
set(CXX_SOURCE_FILE_EXTENSION ".cpp")
set(CXX_MODULE_FILE_EXTENSION ".cppm")
set(MWC_DEBUG 1)
set(MWC_LOG 1)

# project directory paths
set(ROOT_DIR ${PROJECT_SOURCE_DIR})
set(SOURCE_DIR ${ROOT_DIR}/source)
set(SOURCE_PROJECT_DIR ${SOURCE_DIR}/${PROJECT_NAME})
set(SOURCE_STD_DIR ${SOURCE_DIR}/std)
set(MODULE_DIR ${SOURCE_DIR})
set(LOG_DIR ${ROOT_DIR}/log)

# if logging is enabled, attempt to find the logging directory, if not present, create one
if (${MWC_LOG})
    message(CHECK_START "logging is enabled, attempting to find the log directory...")
    if (EXISTS ${LOG_DIR})
        message(CHECK_PASS "logging directory found: " ${LOG_DIR})
    else ()
        message(CHECK_FAIL "logging directory not found, creating one at: " ${LOG_DIR})
        make_directory(${LOG_DIR})
    endif ()
endif ()

# attempt to find the std module file
#[[if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(STD_MODULE_FILE "std.cc")
else()
    #set(STD_MODULE_FILE "std.cppm")
    set(STD_MODULE_FILE "std.cc")
endif()
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
#target_link_libraries(std PUBLIC ${ROOT_DIR}/libstdc++exp.a)]]


# vulkan library
#[[
add_library(vulkan)
find_package(VulkanHeaders CONFIG REQUIRED)
get_target_property(VULKAN_INCLUDE_DIR Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
target_compile_definitions(vulkan PRIVATE VULKAN_HPP_NO_EXCEPTIONS)
target_include_directories(vulkan PRIVATE ${VULKAN_INCLUDE_DIR})
target_sources(vulkan PUBLIC FILE_SET CXX_MODULES FILES ${VULKAN_INCLUDE_DIR}/vulkan/vulkan.cppm)]]

# project executable
add_executable(${PROJECT_NAME})
# project include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})
# project module sources
file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS ${MODULE_DIR}/*${CXX_MODULE_FILE_EXTENSION})
#file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS ${MODULE_DIR}/*test.cppm)
target_sources(${PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES ${MODULE_SOURCES})
# project sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${SOURCE_DIR}/*${CXX_SOURCE_FILE_EXTENSION})
target_sources(${PROJECT_NAME} PRIVATE
        ${SOURCES}
)
# project compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<IF:${MWC_LOG}, MWC_LOG_DIR="${LOG_DIR}", MWC_LOG_DIR="">
)
# project libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        #std
        #stdc++exp
        #${ROOT_DIR}/libstdc++exp.a
        #"-lstdc++exp"
        #vulkan
)
target_link_options(${PROJECT_NAME} PUBLIC -static -lc++ -lc++abi -lc++experimental -fuse-ld=lld)

