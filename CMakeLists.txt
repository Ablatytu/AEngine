cmake_minimum_required(VERSION 3.25)

project(AEngineProject LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${CONFIG}" CONFIG_UPPER)

    set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER}
        "${CMAKE_BINARY_DIR}/bin/${CONFIG}"
    )

    set(
        CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER}
        "${CMAKE_BINARY_DIR}/bin/${CONFIG}"
    )

    set(
        CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER}
        "${CMAKE_BINARY_DIR}/lib/${CONFIG}"
    )
endforeach()

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    yaml_cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 0.8.0
)

FetchContent_MakeAvailable(yaml_cpp)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v6.0.2
)

FetchContent_MakeAvailable(assimp)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm GIT_TAG 1.0.2)

FetchContent_MakeAvailable(glm)

add_library(glad STATIC
    "${CMAKE_SOURCE_DIR}/AEngine/APIs/glad.c"
)

target_include_directories(glad
    PUBLIC
        "${CMAKE_SOURCE_DIR}/AEngine/Libraries/include"
)

set(IMGUI_DIR
    "${CMAKE_SOURCE_DIR}/AEditor/imGUI"
)

add_library(imgui STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"

    "${IMGUI_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/imgui_impl_opengl3.cpp"

    "${IMGUI_DIR}/ImGuizmo.cpp"
)

target_include_directories(imgui
    PUBLIC
        "${IMGUI_DIR}"
)

target_link_libraries(imgui
    PUBLIC
        glfw
        glad
)

file(GLOB_RECURSE AENGINE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/AEngine/*.cpp"
)

add_library(AEngine SHARED
    ${AENGINE_SOURCES}
)


if(WIN32)

    list(FILTER AENGINE_SOURCES
        EXCLUDE REGEX
        ".*MacOS.*"
    )

    list(FILTER AENGINE_SOURCES
        EXCLUDE REGEX
        ".*\\.mm$"
    )

elseif(APPLE)

    list(FILTER AENGINE_SOURCES
        EXCLUDE REGEX
        ".*Windows.*"
    )

endif()


target_compile_definitions(AEngine
    PRIVATE
        AENGINE_BUILD_DLL
)

target_include_directories(AEngine
    BEFORE PRIVATE
        "${yaml_cpp_SOURCE_DIR}/include"
)

target_include_directories(AEngine
    PUBLIC
        "${CMAKE_SOURCE_DIR}/AEngine"
        "${CMAKE_SOURCE_DIR}/AEngine/Libraries/include"
)

target_link_libraries(AEngine
    PUBLIC
	glm::glm
    PRIVATE
        glfw
        glad
        yaml-cpp
        assimp::assimp
)

if(WIN32)

    target_compile_definitions(AEngine
        PUBLIC
            AE_PLATFORM_WINDOWS=1
    )

    target_compile_definitions(AEngine
        PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )

    target_link_libraries(AEngine
        PRIVATE
            opengl32
    )

elseif(APPLE)

    target_compile_definitions(AEngine
        PUBLIC
            AE_PLATFORM_MACOS=1
    )

    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)

    target_link_libraries(AEngine
        PRIVATE
            ${COCOA_FRAMEWORK}
            ${IOKIT_FRAMEWORK}
            ${COREVIDEO_FRAMEWORK}
            ${OPENGL_FRAMEWORK}
    )

endif()


file(GLOB_RECURSE AEDITOR_SOURCES
    CONFIGURE_DEPENDS

    "${CMAKE_SOURCE_DIR}/AEditor/*.cpp"
)

list(FILTER AEDITOR_SOURCES
    EXCLUDE REGEX
    ".*/imGUI/.*"
)

add_executable(AEditor
    ${AEDITOR_SOURCES}
)

target_compile_definitions(AEditor
    PRIVATE
        AEDITOR_WORKING_DIR="${CMAKE_SOURCE_DIR}/AEditor"
)

set_target_properties(AEditor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/AEditor"
)

target_include_directories(AEditor
    PRIVATE
        "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/AEngine/Libraries/include"
        "${IMGUI_DIR}"
)

target_link_libraries(AEditor
    PRIVATE
        AEngine
        imgui
)


file(
    GLOB_RECURSE GAME_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Game/*.cpp"
    "${CMAKE_SOURCE_DIR}/Game/*.c"
)

add_library(Game
    SHARED
    ${GAME_SOURCES}
)

target_include_directories(Game
    PRIVATE
        "${CMAKE_SOURCE_DIR}/Game"
        "${CMAKE_SOURCE_DIR}/GameAPI"
)

set_target_properties(Game
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME "Game"
)

add_dependencies(AEditor Game)


if(APPLE)

    set_target_properties(AEngine PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )

    set_target_properties(AEditor PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )

    set_target_properties(Game PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )

endif()

if(MSVC)

    set_property(
        DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"
        PROPERTY
        VS_STARTUP_PROJECT AEditor
    )

endif()