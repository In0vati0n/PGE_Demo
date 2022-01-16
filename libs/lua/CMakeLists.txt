file(GLOB SRC_FILES "*.c")

set(PROJ_NAME Lua)

project(${PROJ_NAME})

file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/*.h")

source_group("Headers" FILES ${HEADERS})

if (WIN32)
    if (LUA_BUILD_AS_DLL)
        add_library(
            ${PROJ_NAME}
            SHARED
            ${SRC_FILES}
            ${HEADERS}
        )

        target_include_directories(${PROJ_NAME} PUBLIC ./src)
        set_property(TARGET ${PROJ_NAME} PROPERTY COMPILE_DEFINITIONS LUA_BUILD_AS_DLL=1)

        if (CMAKE_BUILD_TYPE MATCHES "Debug")
            add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/Debug/Lua.dll" "${ROOT_SOURCE_DIR}/bin/${ROOT_PROJ_NAME}/Debug/Lua.dll")
        else (CMAKE_BUILD_TYPE MATCHES "Debug")
            add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "./build/Release/Lua.dll" "${ROOT_SOURCE_DIR}/bin/${ROOT_PROJ_NAME}/Release/Lua.dll")
        endif (CMAKE_BUILD_TYPE MATCHES "Debug")
    else (LUA_BUILD_AS_DLL)
        add_library(
            ${PROJ_NAME}
            STATIC
            ${SRC_FILES}
            ${HEADERS}
        )
    endif (LUA_BUILD_AS_DLL)

else (WIN32)
    add_library(
        ${PROJ_NAME}
        STATIC
        ${SRC_FILES}
        ${HEADERS}
    )
endif (WIN32)