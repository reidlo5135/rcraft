cmake_minimum_required(VERSION 3.31)
project(rcraft_viz)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Qt5_DIR "C:/Qt/Qt5.12.4/5.12.4/mingw73_64/lib/cmake/Qt5")
set(QT_BIN_PATH "C:/Qt/Qt5.12.4/5.12.4/mingw73_64/bin")
set(QT_PLUGINS_PATH "C:/Qt/Qt5.12.4/5.12.4/mingw73_64/plugins")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV_LIBS : ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS : ${OpenCV_INCLUDE_DIRS}")
else()
    message(STATUS "OpenCV Not Found")
endif()

set(LIBS_NAME "rcraft_libs")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../${LIBS_NAME}")
find_package(rcraft_path_planner REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE RCRAFT_VIZ_HEADERS include/rcraft/**/*.hpp)
file(GLOB_RECURSE RCRAFT_VIZ_SRC src/*.cpp)
message(STATUS "${RCRAFT_VIZ_SRC}")

set(EXEC_NAME ${PROJECT_NAME})
add_executable(${EXEC_NAME} ${RCRAFT_VIZ_SRC} ${RCRAFT_VIZ_HEADERS})
target_link_libraries(${EXEC_NAME}
        Qt5::Core
        Qt5::Widgets
        rcraft_path_planner::rcraft_path_planner
        ${OpenCV_LIBS}
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    if (EXISTS "${QT_PLUGINS_PATH}/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_PLUGINS_PATH}/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_BIN_PATH}/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
    add_custom_command(TARGET ${EXEC_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/../rcraft_libs/bin/librcraft_path_planner.dll"
            "$<TARGET_FILE_DIR:rcraft_viz>"
    )
endif ()