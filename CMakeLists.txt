cmake_minimum_required(VERSION 3.6)

set(APP_NAME CardPileMatch)
project(${APP_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COCOS2DX_ROOT
    "${CMAKE_SOURCE_DIR}/vendor/cocos2d-x-3.17.2"
    CACHE PATH "Path to cocos2d-x 3.17.2 inside this workspace")

if(NOT EXISTS "${COCOS2DX_ROOT}/cocos")
    message(FATAL_ERROR "cocos2d-x 3.17.2 was not found. Run tools/setup_cocos2dx_3_17.ps1 first.")
endif()

set(COCOS2DX_ROOT_PATH ${COCOS2DX_ROOT})
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)
add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)

set(GAME_RES_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

set(GAME_SOURCES
    Classes/AppDelegate.cpp
    Classes/MainScene.cpp
    Classes/Models/Card.cpp
    Classes/Models/GameState.cpp
    Classes/Views/CardView.cpp
    proj.win32/main.cpp
    ${common_res_files}
)

set(GAME_HEADERS
    Classes/AppDelegate.h
    Classes/MainScene.h
    Classes/Models/Card.h
    Classes/Models/GameState.h
    Classes/Views/CardView.h
    proj.win32/main.h
)

add_executable(${APP_NAME} WIN32 ${GAME_SOURCES} ${GAME_HEADERS})
target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
    PRIVATE Classes
    PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

setup_cocos_app_config(${APP_NAME})
if(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    set(APP_RES_DIR "$<TARGET_FILE_DIR:${APP_NAME}>/Resources")
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
