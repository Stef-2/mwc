cmake_minimum_required(VERSION 4.0.3 FATAL_ERROR)
include(CMakePrintHelpers)
include(FetchContent)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_STANDARD_REQUIRED FALSE)
set(CMAKE_CXX_MODULE_STD TRUE)

project(mwc LANGUAGES CXX)

# project configuration
set(CXX_HEADER_FILE_EXTENSION ".hpp")
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

# vulkan library
add_library(vulkan)
find_package(VulkanHeaders CONFIG REQUIRED)
get_target_property(VULKAN_INCLUDE_DIR Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
target_compile_definitions(vulkan PRIVATE
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_ASSERT=contract_assert
        VULKAN_HPP_ASSERT_ON_RESULT=VULKAN_HPP_ASSERT
)
target_sources(vulkan PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${VULKAN_INCLUDE_DIR} FILES ${VULKAN_INCLUDE_DIR}/vulkan/vulkan.cppm)

# vkfw library
set(VKFW_BUILD_CXX_MODULE 1)
FetchContent_Declare(vkfw GIT_REPOSITORY https://github.com/Cvelth/vkfw GIT_TAG main)
FetchContent_MakeAvailable(vkfw)
target_compile_definitions(vkfw INTERFACE
        VKFW_NO_EXCEPTIONS
        VKFW_NO_STRUCT_CONSTRUCTORS
        VKFW_ASSERT=contract_assert
        VKFW_ASSERT_ON_RESULT=VKFW_ASSERT
        assert=VKFW_ASSERT
)

# project executable
add_executable(${PROJECT_NAME})
# project include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})
# project module sources
file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS ${MODULE_DIR}/*${CXX_MODULE_FILE_EXTENSION})
target_sources(${PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES BASE_DIRS ${MODULE_DIR} FILES ${MODULE_SOURCES})
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
        vulkan
        vkfw-module
)
target_link_options(${PROJECT_NAME} PUBLIC
)