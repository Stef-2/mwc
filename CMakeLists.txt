cmake_minimum_required(VERSION 4.0.3 FATAL_ERROR)
include(CMakePrintHelpers)
include(FetchContent)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_LINKER_TYPE LLD)
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
set(DATA_DIR ${ROOT_DIR}/data)
set(DATA_IMAGE_DIR ${DATA_DIR}/image)
set(DATA_MESH_DIR ${DATA_DIR}/mesh)
set(DATA_SHADER_DIR ${DATA_DIR}/shader)

# third party dependencies
set(OPEN_USD_INSTALLATION_DIR "/opt/OpenUSD/")

# if logging is enabled, attempt to find the logging directory, if not present, generate one
if (${MWC_LOG})
    message(CHECK_START "logging is enabled, attempting to find the log directory...")
    if (EXISTS ${LOG_DIR})
        message(CHECK_PASS "logging directory found: " ${LOG_DIR})
    else ()
        message(CHECK_FAIL "logging directory not found, generating one at: " ${LOG_DIR})
        make_directory(${LOG_DIR})
    endif ()
endif ()

if (NOT EXISTS ${DATA_IMAGE_DIR})
    make_directory(${DATA_IMAGE_DIR})
endif ()
if (NOT EXISTS ${DATA_MESH_DIR})
    make_directory(${DATA_IMAGE_DIR})
endif ()
if (NOT EXISTS ${DATA_SHADER_DIR})
    make_directory(${DATA_SHADER_DIR})
endif ()

set(MWC_ASSERT assert)
add_compile_definitions(VULKAN_HPP_NO_EXCEPTIONS)
add_compile_definitions(VULKAN_HPP_RAII_NO_EXCEPTIONS)
add_compile_definitions(VULKAN_HPP_ENABLE_STD_MODULE)
add_compile_definitions(VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
add_compile_definitions(VKFW_NO_EXCEPTIONS)
add_compile_definitions(VKFW_NO_STRUCT_CONSTRUCTORS)

#-Wno-error=incompatible-pointer-types
# maybe needed after with clang 22+

#add_compile_definitions(VULKAN_HPP_NO_EXCEPTIONS 1)
# vulkan library

#add_library(VulkanHppModule)
#find_package(Vulkan REQUIRED)
#[[
target_compile_definitions(VulkanHppModule PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_RAII_NO_EXCEPTIONS
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_ENABLE_STD_MODULE

        VULKAN_HPP_ASSERT=${MWC_ASSERT}
        VULKAN_HPP_ASSERT_ON_RESULT=VULKAN_HPP_ASSERT
)
target_compile_definitions(VulkanHppModule PRIVATE
        VULKAN_HPP_STD_MODULE=std.compat
)
target_sources(VulkanHppModule PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${Vulkan_INCLUDE_DIR} FILES ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm)
target_compile_features(VulkanHppModule PUBLIC cxx_std_26)
target_link_libraries(VulkanHppModule PUBLIC Vulkan::Headers)
]]
# vkfw library
#add_library(vkfw_module)

#[[
target_include_directories(vkfw_module PRIVATE ${vkfw_SOURCE_DIR}/include)
target_sources(vkfw_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${vkfw_SOURCE_DIR} FILES ${vkfw_SOURCE_DIR}/include/vkfw/vkfw.cppm)
target_compile_features(vkfw_module PUBLIC cxx_std_26)
target_compile_definitions(vkfw_module PRIVATE
        VKFW_ENABLE_VULKAN_HPP_MODULE
        VKFW_NO_EXCEPTIONS
        VKFW_NO_STRUCT_CONSTRUCTORS
        VKFW_ENABLE_STD_MODULE
        #VKFW_ASSERT=${MWC_ASSERT}
        #VKFW_ASSERT_ON_RESULT=VKFW_ASSERT
        #assert=VKFW_ASSERT
)

target_link_libraries(vkfw_module PRIVATE VulkanHppModule)
]]
# vma library
#

#add_library(vma_module)
#find_package(VulkanHeaders CONFIG REQUIRED)
FetchContent_Declare(vma GIT_REPOSITORY https://github.com/YaaZ/VulkanMemoryAllocator-Hpp)
FetchContent_MakeAvailable(vma)

set(VKFW_ENABLE_STD_MODULE 1)
set(VKFW_BUILD_CXX_MODULE 1)
set(VKFW_NO_INCLUDE_VULKAN_HPP 1)
set(VKFW_ENABLE_VULKAN_HPP_MODULE 1)
FetchContent_Declare(
        vkfw
        GIT_REPOSITORY https://github.com/Cvelth/vkfw
        GIT_TAG gcc-16#[[main]]

)
FetchContent_MakeAvailable(vkfw)
#target_compile_definitions(vkfw INTERFACE "-DVKFW_NO_INCLUDE_VULKAN_HPP=1 -DVKFW_ENABLE_VULKAN_HPP_MODULE=1")
#target_compile_options(vkfw INTERFACE "-std=c++26")
target_link_libraries(vkfw INTERFACE Vulkan-HppModule)
find_package(glm CONFIG REQUIRED)
find_package(slang CONFIG REQUIRED)
#[[
target_include_directories(vma_module PRIVATE ${vma_SOURCE_DIR}/include)
target_sources(vma_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${vma_SOURCE_DIR} FILES ${vma_SOURCE_DIR}/include/vk_mem_alloc.cppm)
target_compile_features(vma_module PUBLIC cxx_std_26)
target_compile_definitions(vma_module PRIVATE
        VULKAN_HPP_STD_MODULE=std
        VMA_ENABLE_VULKAN_HPP_MODULE
        VULKAN_HPP_NO_EXCEPTIONS
        VMA_ASSERT=${MWC_ASSERT}
        VMA_ASSERT_ON_RESULT=VKFW_ASSERT
        assert=VMA_ASSERT
)
target_link_libraries(vma_module PRIVATE VulkanHppModule)
]]

#[[
target_include_directories(fast_gltf_module PRIVATE ${fastgltf_SOURCE_DIR}/include)
target_sources(fast_gltf_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${fastgltf_SOURCE_DIR} FILES ${fastgltf_SOURCE_DIR}/src/fastgltf.ixx)
target_compile_features(fast_gltf_module PUBLIC cxx_std_26)
target_compile_definitions(fast_gltf_module PRIVATE
        FASTGLTF_COMPILE_AS_CPP20
        FASTGLTF_USE_STD_MODULE
)
target_link_libraries(fast_gltf_module PRIVATE fastgltf)]]

find_package(imgui CONFIG REQUIRED)
find_package(Python REQUIRED COMPONENTS Development)
find_package(pxr CONFIG REQUIRED NO_DEFAULT_PATH HINTS ${OPEN_USD_INSTALLATION_DIR})
if (pxr_FOUND)
    message("OpenUSD installation found at: " ${pxr_DIR})
    cmake_print_properties(
            TARGETS usd
            PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES
            INCLUDE_DIRECTORIES
    )
endif ()

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
        MWC_DATA_IMAGE_DIR="${DATA_IMAGE_DIR}"
        MWC_DATA_MESH_DIR="${DATA_MESH_DIR}"
        MWC_DATA_SHADER_DIR="${DATA_SHADER_DIR}"
        MWC_ASSERT=${MWC_ASSERT}
)
# project libraries
#target_link_directories(${PROJECT_NAME} PUBLIC "/home/billy/Downloads/usd.py312.manylinux_2_35_x86_64.usdview.release-v25.08.71e038c1/lib/")
target_link_libraries(${PROJECT_NAME} PUBLIC
        #VulkanHppModule
        #Vulkan
        vulkan
        Vulkan-HppModule
        VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-HppModule
        glm::glm
        #VMA-Hpp
        glfw
        #Vulkan
        #vkfw_module
        #usd
        #usd_ms
        vkfw-module
        Python::Python
        usd
        /opt/OpenUSD/lib/libusd_ms.so
        #usd_ms
        #vma
        #python
        imgui::imgui
        #fast_gltf_module
        slang::slang
)