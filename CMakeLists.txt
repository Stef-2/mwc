cmake_minimum_required(VERSION 4.0.3 FATAL_ERROR)
include(CMakePrintHelpers)
include(FetchContent)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_STANDARD_REQUIRED FALSE)
set(CMAKE_CXX_MODULE_STD TRUE)

project(mwc LANGUAGES CXX)
include_directories(/opt/compiler-explorer/clang-ericwf-contracts-trunk-20250726/include/x86_64-unknown-linux-gnu/c++/v1/)
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
add_library(VulkanHppModule)
find_package(Vulkan REQUIRED)
target_compile_definitions(VulkanHppModule PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_RAII_NO_EXCEPTIONS
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        #VULKAN_HPP_ENABLE_STD_MODULE
        #VULKAN_HPP_STD_MODULE=std.compat
        VULKAN_HPP_ASSERT=contract_assert
        VULKAN_HPP_ASSERT_ON_RESULT=VULKAN_HPP_ASSERT
)
target_sources(VulkanHppModule PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${Vulkan_INCLUDE_DIR} FILES ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm)
target_compile_features(VulkanHppModule PUBLIC cxx_std_26)
target_link_libraries(VulkanHppModule PUBLIC Vulkan::Vulkan)

# vkfw library
add_library(vkfw_module)
FetchContent_Declare(vkfw GIT_REPOSITORY https://github.com/Cvelth/vkfw GIT_TAG main)
FetchContent_MakeAvailable(vkfw)
target_include_directories(vkfw_module PRIVATE ${vkfw_SOURCE_DIR}/include)
target_sources(vkfw_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${vkfw_SOURCE_DIR} FILES ${vkfw_SOURCE_DIR}/include/vkfw/vkfw.cppm)
target_compile_features(vkfw_module PUBLIC cxx_std_26)
target_compile_definitions(vkfw_module PRIVATE
        VKFW_ENABLE_VULKAN_HPP_MODULE
        VKFW_NO_EXCEPTIONS
        VKFW_NO_STRUCT_CONSTRUCTORS
        VKFW_ENABLE_STD_MODULE
        VKFW_ASSERT=contract_assert
        VKFW_ASSERT_ON_RESULT=VKFW_ASSERT
        assert=VKFW_ASSERT
)
target_link_libraries(vkfw_module PRIVATE VulkanHppModule)

# vma library
add_library(vma_module)
FetchContent_Declare(vma GIT_REPOSITORY https://github.com/YaaZ/VulkanMemoryAllocator-Hpp)
FetchContent_MakeAvailable(vma)
target_include_directories(vma_module PRIVATE ${vma_SOURCE_DIR}/include)
target_sources(vma_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${vma_SOURCE_DIR} FILES ${vma_SOURCE_DIR}/src/vk_mem_alloc.cppm)
target_compile_features(vma_module PUBLIC cxx_std_26)
target_compile_definitions(vma_module PRIVATE
        VULKAN_HPP_STD_MODULE=std
        VMA_ENABLE_VULKAN_HPP_MODULE
        VULKAN_HPP_NO_EXCEPTIONS
        VMA_ASSERT=contract_assert
        VMA_ASSERT_ON_RESULT=VKFW_ASSERT
        assert=VMA_ASSERT
)
target_link_libraries(vma_module PRIVATE VulkanHppModule)

# fastgltf library
add_library(fast_gltf_module)
FetchContent_Declare(fastgltf GIT_REPOSITORY https://github.com/spnda/fastgltf GIT_TAG main)
FetchContent_MakeAvailable(fastgltf)
target_include_directories(fast_gltf_module PRIVATE ${fastgltf_SOURCE_DIR}/include)
target_sources(fast_gltf_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${fastgltf_SOURCE_DIR} FILES ${fastgltf_SOURCE_DIR}/src/fastgltf.ixx)
target_compile_features(fast_gltf_module PUBLIC cxx_std_26)
target_compile_definitions(fast_gltf_module PRIVATE
        FASTGLTF_COMPILE_AS_CPP20
        #FASTGLTF_USE_STD_MODULE
)
target_link_libraries(fast_gltf_module PRIVATE fastgltf)

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
        VulkanHppModule
        glfw
        vkfw_module
        vma_module
        imgui
        fast_gltf_module
)