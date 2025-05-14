cmake_minimum_required(VERSION 3.16)
project(TypicalTools LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
    Gui Widgets OpenGL Quick QuickControls2
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    src/main.cpp
)

# 查找所有源文件和头文件
file(GLOB_RECURSE SOURCE_FILES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


if(QT_VERSION VERSION_LESS 5.15)
    qt5_add_resources(PROJECT_SOURCES qml.qrc)
elseif(QT_VERSION VERSION_LESS 6.2)
    qt_add_resources(PROJECT_SOURCES qml.qrc)
endif()

qt_add_executable(${PROJECT_NAME} ${SOURCE_FILES_LIST} ${HEADER_FILES} ${PROJECT_SOURCES}) # ${PROJECT_SOURCES}

if(QT_VERSION VERSION_GREATER_EQUAL 6.2)
    qt_add_qml_module(${PROJECT_NAME}
        URI typicaltools
        VERSION 1.0
        RESOURCE_PREFIX "/qt/qml/"
        QML_FILES
            main.qml
    )
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Widgets
)


if(Qt6_FOUND)
    qt_import_qml_plugins(${PROJECT_NAME})
endif()

