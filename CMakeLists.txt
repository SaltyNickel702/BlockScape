cmake_minimum_required(VERSION 3.31)

project(BlockScape)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(PRODUCTION_BUILD True)

if (PRODUCTION_BUILD)
    set(EXE_NAME "BlockScape")
else()
    set(EXE_NAME "out")
endif()


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
        add_executable(${EXE_NAME} WIN32 ${SOURCES} ${APP_ICON_RESOURCE_WINDOWS}) # add WIN32 to create the windows application w/out console
    else()
        add_executable(${EXE_NAME} ${SOURCES} ${APP_ICON_RESOURCE_WINDOWS})
    endif()
else() #MACOS
    if (PRODUCTION_BUILD)
        # Add icon (macOS uses .icns, so you must convert BlockScape.png to favicon.icns)
        set(APP_ICON "${CMAKE_SOURCE_DIR}/assets/textures/BlockScape.icns")

        #Bundle into .app format
        add_executable(${EXE_NAME} MACOSX_BUNDLE ${SOURCES})

        # Bundle properties
        set_target_properties(${EXE_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME ${EXE_NAME}
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist"
            RESOURCE "${CMAKE_SOURCE_DIR}/assets;${CMAKE_SOURCE_DIR}/saves"
        )

        # Copy required folders to .app bundle after build
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/assets"
                "$<TARGET_FILE_DIR:MyApp>/../Resources/assets"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/saves"
                "$<TARGET_FILE_DIR:MyApp>/../Resources/saves"
        )

    else()
        add_executable(${EXE_NAME} ${SOURCES})
    endif()
endif()


include_directories(
    "${CMAKE_SOURCE_DIR}/include/"
    # "${CMAKE_SOURCE_DIR}/include/SDL2"
    # "${CMAKE_SOURCE_DIR}/include/SDL2_mixer"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${EXE_NAME}
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
    target_link_libraries(${EXE_NAME}
        "-framework CoreFoundation"
        "-framework IOKit"
        "-framework Cocoa"
    )
    target_link_libraries(${EXE_NAME}
        "${CMAKE_SOURCE_DIR}/statLib/libglfw3MAC.a"
        "${CMAKE_SOURCE_DIR}/statLib/libgladMAC.a"
    )
endif()

# Library paths (already handled above with link_directories)