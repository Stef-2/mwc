cmake_minimum_required(VERSION 4.4 FATAL_ERROR)
include(CMakePrintHelpers)
include(FetchContent)

# cmake configuration
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
    if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 4.4)
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD f35a9ac6-8463-4d38-8eec-5d6008153e7d)
    elseif(${CMAKE_VERSION} VERSION_GREATER_EQUAL 4.3)
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 451f2fe2-a8a2-47c3-bc32-94786d8fc91b)
    elseif(${CMAKE_VERSION} VERSION_GREATER_EQUAL 4.2)
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD d0edc3af-4c50-42ea-a356-e2862fe7a444)
    elseif(${CMAKE_VERSION} VERSION_GREATER_EQUAL 4.0)
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 1942b4fa-b2c5-4546-9385-83f254070067)
    elseif(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.30)
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 0e5b6991-d74f-4b3d-a41c-cf096e0b2508)
    endif()
endif()
set(CMAKE_CXX_MODULE_STD TRUE)

message("using cmake version: ${CMAKE_VERSION}")
message("enabling CMAKE_EXPERIMENTAL_CXX_IMPORT_STD: ${CMAKE_EXPERIMENTAL_CXX_IMPORT_STD}")

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
set(EXTERNAL_SOURCE_DIR ${ROOT_DIR}/external)
set(SOURCE_STD_DIR ${SOURCE_DIR}/std)
set(MODULE_DIR ${SOURCE_DIR})
set(LOG_DIR ${ROOT_DIR}/log)
set(DATA_DIR ${ROOT_DIR}/data)
set(DATA_IMAGE_DIR ${DATA_DIR}/image)
set(DATA_MESH_DIR ${DATA_DIR}/mesh)
set(DATA_SHADER_DIR ${DATA_DIR}/shader)

set(FETCHCONTENT_BASE_DIR ${EXTERNAL_SOURCE_DIR})

# compile features
add_compile_options(-std=c++26 -fcontracts -freflection -fmodules -fstrict-enums --embed-dir=${SOURCE_DIR})

# if logging is enabled, attempt to find the logging directory, if not present, generate one
if (${MWC_LOG})
    message(CHECK_START "logging is enabled, attempting to find the log directory...")
    if (EXISTS ${LOG_DIR})
        message(CHECK_PASS "logging directory found: " ${LOG_DIR})
    else ()
        message(CHECK_FAIL "logging directory not found, generating one at: " ${LOG_DIR})
        file(MAKE_DIRECTORY ${LOG_DIR})
    endif ()
endif ()

if (NOT EXISTS ${DATA_IMAGE_DIR})
    file(MAKE_DIRECTORY ${DATA_IMAGE_DIR})
endif ()
if (NOT EXISTS ${DATA_MESH_DIR})
    file(MAKE_DIRECTORY ${DATA_IMAGE_DIR})
endif ()
if (NOT EXISTS ${DATA_SHADER_DIR})
    file(MAKE_DIRECTORY ${DATA_SHADER_DIR})
endif ()

# assert
set(MWC_ASSERT "contract_assert")

# vulkan library
find_package(Vulkan REQUIRED)
if (NOT TARGET Vulkan::HppModule)
    add_library(vulkan_cxx_module OBJECT)
    add_library(Vulkan::HppModule ALIAS vulkan_cxx_module)
    target_sources(vulkan_cxx_module PUBLIC
            FILE_SET CXX_MODULES
            BASE_DIRS ${Vulkan_INCLUDE_DIR}
            FILES
            ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm
            ${Vulkan_INCLUDE_DIR}/vulkan/vulkan_video.cppm
    )
    target_compile_definitions(vulkan_cxx_module PUBLIC
            VULKAN_HPP_NO_EXCEPTIONS
            VULKAN_HPP_RAII_NO_EXCEPTIONS
            VULKAN_HPP_NO_TO_STRING
            VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
            VULKAN_HPP_ENABLE_STD_MODULE
            VULKAN_HPP_STD_MODULE=std.compat
            VULKAN_HPP_ASSERT=${MWC_ASSERT}
    )
    target_compile_options(vulkan_cxx_module PRIVATE -w)
    target_include_directories(vulkan_cxx_module PUBLIC ${Vulkan_INCLUDE_DIR})
endif ()

# vkfw library
FetchContent_Declare(
        vkfw
        GIT_REPOSITORY https://github.com/Cvelth/vkfw
        GIT_TAG main
        SOURCE_SUBDIR "workaround for CMAKE_EXPERIMENTAL_CXX_IMPORT_STD"
)
FetchContent_MakeAvailable(vkfw)
add_library(vkfw_cxx_module OBJECT)
target_sources(vkfw_cxx_module PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${vkfw_SOURCE_DIR}
        FILES
        ${vkfw_SOURCE_DIR}/include/vkfw/vkfw.cppm
)
target_compile_definitions(vkfw_cxx_module PUBLIC
        VKFW_NO_EXCEPTIONS
        VKFW_NO_STRUCT_CONSTRUCTORS
        VKFW_ENABLE_STD_MODULE
        VKFW_BUILD_WITH_STD_MODULE
        VKFW_ENABLE_VULKAN_HPP_MODULE
        VKFW_BUILD_WITH_VULKAN_MODULE
        VKFW_BUILD_MODULE_VULKAN_DYNAMIC
        VKFW_ASSERT=${MWC_ASSERT}
        VKFW_INLINE=inline
)
target_compile_options(vkfw_cxx_module PRIVATE -w)
target_include_directories(vkfw_cxx_module PUBLIC ${vkfw_SOURCE_DIR}/include/vkfw)
target_link_libraries(vkfw_cxx_module PRIVATE vulkan_cxx_module)

# vma library
FetchContent_Declare(vma GIT_REPOSITORY
        https://github.com/YaaZ/VulkanMemoryAllocator-Hpp
        SOURCE_SUBDIR "workaround for CMAKE_EXPERIMENTAL_CXX_IMPORT_STD"
)
FetchContent_MakeAvailable(vma)
add_library(vma_cxx_module OBJECT)
target_include_directories(vma_cxx_module PRIVATE
        ${vma_SOURCE_DIR}/include
        ${vma_SOURCE_DIR}/VulkanMemoryAllocator/include
)
target_sources(vma_cxx_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${vma_SOURCE_DIR} FILES ${vma_SOURCE_DIR}/include/vk_mem_alloc.cppm)
target_compile_definitions(vma_cxx_module PRIVATE
        VULKAN_HPP_STD_MODULE=std
        VULKAN_HPP_NO_EXCEPTIONS
        VMA_ENABLE_VULKAN_HPP_MODULE
        #VMA_ASSERT=${MWC_ASSERT}
        assert=VMA_ASSERT
)
target_compile_options(vma_cxx_module PRIVATE -w)
target_link_libraries(vma_cxx_module PRIVATE vulkan_cxx_module)

#glm library
find_package(glm CONFIG REQUIRED)
get_target_property(glm_INCLUDE_DIR glm::glm-header-only INTERFACE_INCLUDE_DIRECTORIES)
add_library(glm_cxx_module OBJECT)
target_include_directories(glm_cxx_module PRIVATE
    ${vma_SOURCE_DIR}
)
target_compile_definitions(glm_cxx_module PRIVATE
    GLM_ENABLE_EXPERIMENTAL
    GLM_GTC_INLINE_NAMESPACE
    GLM_EXT_INLINE_NAMESPACE
    GLM_GTX_INLINE_NAMESPACE
)
target_compile_options(glm_cxx_module PRIVATE -w)
target_sources(glm_cxx_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${glm_INCLUDE_DIR} FILES ${glm_INCLUDE_DIR}/glm/glm.cppm)


# fastgltf library
#[[
add_library(fast_gltf_module)
FetchContent_Declare(fastgltf GIT_REPOSITORY https://github.com/spnda/fastgltf GIT_TAG main)
FetchContent_MakeAvailable(fastgltf)
target_include_directories(fast_gltf_module PRIVATE ${fastgltf_SOURCE_DIR}/include)
target_sources(fast_gltf_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${fastgltf_SOURCE_DIR} FILES ${fastgltf_SOURCE_DIR}/src/fastgltf.ixx)
target_compile_features(fast_gltf_module PUBLIC cxx_std_26)
target_compile_definitions(fast_gltf_module PRIVATE
        FASTGLTF_COMPILE_AS_CPP20
        FASTGLTF_USE_STD_MODULE
)
target_link_libraries(fast_gltf_module PRIVATE fastgltf)]]
#[[
]]
find_package(imgui REQUIRED)
find_package(pxr CONFIG REQUIRED HINTS ${OPEN_USD_INSTALL_DIR})
find_package(slang CONFIG REQUIRED)
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
target_link_libraries(${PROJECT_NAME} PUBLIC
        vulkan_cxx_module
        vkfw_cxx_module
        vma_cxx_module
        glm_cxx_module
        glfw
        usd
        usd_ms
        imgui::imgui
        slang::slang
)