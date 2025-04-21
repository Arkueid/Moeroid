set(CMAKE_PREFIX_PATH "D:/Qt/6.8.2/msvc2022_64/")
set(CMAKE_AUTOMOC ON)   # 自动处理 Q_OBJECT 宏的元对象编译（moc）
set(CMAKE_AUTOUIC ON)   # 自动编译 UI 文件（.ui → .h）
set(CMAKE_AUTORCC ON)   # 自动编译资源文件（.qrc → .cpp）

find_package(Qt6 COMPONENTS Core Widgets Gui OpenGLWidgets LinguistTools REQUIRED)

add_executable(${PROJECT_NAME}
    Main.cpp
    
    Win/Input.cpp
    Win/Live2DWidget.cpp
    Win/Output.cpp
    Win/Systray.cpp
    Win/HistoryView.cpp
    Win/About.cpp

    Task/LLMTTSWorker.cpp

    Process/PythonProcess.cpp

    LipSync/LAppWavFileHandler.cpp
    LipSync/Lipsync.cpp
    
    Config/MoeConfig.cpp
    Config/StickState.cpp

    Util/CubismHelper.cpp

    Sqlite/sqlite3.c
    Sqlite/SQLite.cpp

    Moe/Moe.hpp
    Moe/Moe.cpp

    logo.rc
)

target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus" "/permissive-" "/utf-8")

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE CSM_TARGET_WIN_GL UNICODE _UNICODE
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QTINC}
    ${PROJECT_ROOT}/Framework/src
    ${PROJECT_ROOT}/Core/include
    ${PROJECT_ROOT}/Glad/include
    ${PROJECT_ROOT}/Main/src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGLWidgets 
    Main
    winmm.lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_ROOT}/x64/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_ROOT}/x64/Release
)

file(COPY ${PROJECT_ROOT}/Resources/16x16.ico DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(MARKER $<IF:$<CONFIG:DEBUG>,${PROJECT_ROOT}/x64/Debug/.deployed,${PROJECT_ROOT}/x64/Release/.deployed>)

# 获取 windeployqt6 工具的路径
get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt6 HINTS "${_qt_bin_dir}")

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
        if not exist ${MARKER} (${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}> && echo done. > ${MARKER}) else (echo deployed)
)

set(ts_files
    ${CMAKE_CURRENT_SOURCE_DIR}/Translations/moe_zh_CN.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/Translations/moe_en.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/Translations/moe_ja_JP.ts
)

qt6_add_translations(${PROJECT_NAME} TS_FILES ${ts_files})

