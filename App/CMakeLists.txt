set(CMAKE_PREFIX_PATH "D:/Qt/6.8.2/msvc2022_64/")
set(CMAKE_AUTOMOC ON)   # 自动处理 Q_OBJECT 宏的元对象编译（moc）
set(CMAKE_AUTOUIC ON)   # 自动编译 UI 文件（.ui → .h）
set(CMAKE_AUTORCC ON)   # 自动编译资源文件（.qrc → .cpp）
set(CMAKE_INCLUDE_CURRENT_DIR ON)  # 包含生成文件的目录

find_package(Qt6 COMPONENTS Core Widgets Gui OpenGLWidgets REQUIRED)

add_executable(${PROJECT_NAME}
    Main.cpp
    
    Win/Input.cpp
    Win/Live2DWidget.cpp
    Win/Output.cpp
    Win/Systray.cpp

    Task/LLMTTSWorker.cpp

    Process/PythonProcess.cpp

    LipSync/LAppWavFileHandler.cpp
    LipSync/Lipsync.cpp
    
    Config/MoeConfig.cpp
    Config/StickState.cpp

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
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_ROOT}/x64/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_ROOT}/x64/Release
)

file(COPY ${PROJECT_ROOT}/Resources/16x16.ico DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
    ${CMAKE_PREFIX_PATH}/bin/windeployqt6.exe $<TARGET_FILE:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND
    ${CMAKE_PREFIX_PATH}/bin/uic.exe ${CMAKE_CURRENT_SOURCE_DIR}/UI/Input.ui -o ${CMAKE_CURRENT_SOURCE_DIR}/UI/Input.h
)