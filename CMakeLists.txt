cmake_minimum_required(VERSION 3.1)

set(PROJ_NAME PGE_Demo)

project(${PROJ_NAME})

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

option(DEBUG "Debug Version" on)

# Set compile flag
if(WIN32 AND MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG /MTd")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
else()
    if(DEBUG)
        add_definitions(-DDEBUG)
    endif(DEBUG)
endif()

set(PROJ_SOURCE_ROOT ${PROJECT_SOURCE_DIR})

# Find library
if(APPLE)
    #########################################################
    # FIND CARBON
    #########################################################
    find_library(CARBON_LIBRARY Carbon)

    #########################################################
    # FIND PNG
    #########################################################
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIRS})
    link_directories(${PNG_LIBRARY_DIRS})
    add_definitions(${PNG_DEFINITIONS})
    if(NOT PNG_FOUND)
        message(ERROR " PNG not found!")
    endif(NOT PNG_FOUND)

    #########################################################
    # FIND GLUT
    #########################################################
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})
    if(NOT GLUT_FOUND)
        message(ERROR " GLUT not found!")
    endif(NOT GLUT_FOUND)

    #########################################################
    # FIND OPENGL
    #########################################################
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    link_directories(${OpenGL_LIBRARY_DIRS})
    add_definitions(${OpenGL_DEFINITIONS})
    if(NOT OPENGL_FOUND)
        message(ERROR " OpenGL not found!")
    endif(NOT OPENGL_FOUND)
else()
endif()

# Set include path
include_directories(libs/lua)
include_directories(libs/olcPixelGameEngine)

aux_source_directory(. SRC_FILES)
file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/*.h")

source_group("Headers" FILES ${HEADERS})

# Set executeable output path
if (WIN32)
    set(EXECUTABLE_OUTPUT_PATH "${PROJ_SOURCE_ROOT}/bin/${PROJ_NAME}")
else (WIN32)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(EXECUTABLE_OUTPUT_PATH "${PROJ_SOURCE_ROOT}/bin/Debug")
    else (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(EXECUTABLE_OUTPUT_PATH "${PROJ_SOURCE_ROOT}/bin/Release")
    endif (CMAKE_BUILD_TYPE MATCHES "Debug")
endif (WIN32)

add_subdirectory(libs/lua)
add_definitions(-DBUILD_LUA_AS_CLIB)

add_executable(
    ${PROJ_NAME}
    ${SRC_FILES}
    ${HEADERS}
)

if(APPLE)
    target_link_libraries(
        ${PROJ_NAME}
        ${CARBON_LIBRARY}
        ${PNG_LIBRARIES}
        ${GLUT_LIBRARIES}
        ${OPENGL_LIBRARIES}
        Lua
    )
else()
    target_link_libraries(
        ${PROJ_NAME}
        Lua
    )
endif()

# Copy lua files
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${PROJ_SOURCE_ROOT}/_pge.lua" "${ROOT_SOURCE_DIR}/bin/${ROOT_PROJ_NAME}/Debug/_pge.lua")
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${PROJ_SOURCE_ROOT}/game.lua" "${ROOT_SOURCE_DIR}/bin/${ROOT_PROJ_NAME}/Debug/game.lua")
else (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${PROJ_SOURCE_ROOT}/_pge.lua" "${ROOT_SOURCE_DIR}/bin/${ROOT_PROJ_NAME}/Release/_pge.lua")
    add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${PROJ_SOURCE_ROOT}/game.lua" "${ROOT_SOURCE_DIR}/bin/${ROOT_PROJ_NAME}/Release/game.lua")
endif (CMAKE_BUILD_TYPE MATCHES "Debug")
