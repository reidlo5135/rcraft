cmake_minimum_required(VERSION 3.14)
project(rcraft_algorithm VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE RCRAFT_ALGORITHM_SRC src/*.cpp)

set(LIB_NAME ${PROJECT_NAME})

find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
else()
    message(STATUS "OpenCV Not Found")
endif()

add_library(${LIB_NAME} SHARED ${RCRAFT_ALGORITHM_SRC})
target_include_directories(${LIB_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${LIB_NAME} PRIVATE ${OpenCV_LIBS})

add_executable(${PROJECT_NAME}_test src/main.cpp)
target_include_directories(${PROJECT_NAME}_test PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${LIB_NAME})
