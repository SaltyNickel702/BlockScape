cmake_minimum_required(VERSION 3.31)

project(BlockScape)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(PRODUCTION_BUILD False)


# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows") #stuff before adding executable
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_SOURCE_DIR}/app_icon.rc")
endif()


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (PRODUCTION_BUILD)
        add_executable(BlockScape WIN32 ${SOURCES} ${APP_ICON_RESOURCE_WINDOWS}) # add WIN32 to create the windows application w/out console
    else()
        add_executable(out ${SOURCES} ${APP_ICON_RESOURCE_WINDOWS})
    endif()
else()
    add_executable(out ${SOURCES}) # add WIN32 to create the windows application w/out console
endif()


include_directories(
    "${CMAKE_SOURCE_DIR}/include/"
    # "${CMAKE_SOURCE_DIR}/include/SDL2"
    # "${CMAKE_SOURCE_DIR}/include/SDL2_mixer"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(out
        "${CMAKE_SOURCE_DIR}/statLib/libglfw3WIN.a"
        "${CMAKE_SOURCE_DIR}/statLib/libgladWIN.a"
        ws2_32
        # "${CMAKE_SOURCE_DIR}/statLib/SDL2/libSDL2.a"
        # "${CMAKE_SOURCE_DIR}/statLib/SDL2_mixer/libSDL2_mixer.a"
        # -lwinmm
        # -lkernel32
        # -luser32
        # -lgdi32
        # -lwinspool
        # -lshell32
        # -lole32
        # -loleaut32
        # -luuid
        # -lcomdlg32
        # -ladvapi32
        # -limm32
        # -lsetupapi
        # version  # <-- Add this line
    )
    # link_directories(
    #     "${CMAKE_SOURCE_DIR}/statLib/SDL2"
    #     "${CMAKE_SOURCE_DIR}/statLib/SDL2_mixer"
    # )
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
else()
    target_link_libraries(out
        "-framework CoreFoundation"
        "-framework IOKit"
        "-framework Cocoa"
    )
    target_link_libraries(out
        "${CMAKE_SOURCE_DIR}/statLib/libglfw3MAC.a"
        "${CMAKE_SOURCE_DIR}/statLib/libgladMAC.a"
    )
endif()

# Library paths (already handled above with link_directories)