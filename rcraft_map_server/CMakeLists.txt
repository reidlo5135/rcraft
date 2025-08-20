cmake_minimum_required(VERSION 3.31)
project(rcraft_map_server VERSION 0.1.9 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE RCRAFT_MAP_SERVER_SRC src/*.cpp)

set(LIB_NAME ${PROJECT_NAME})

find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV_LIBS : ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS : ${OpenCV_INCLUDE_DIRS}")
else()
    message(STATUS "OpenCV Not Found")
endif()

add_library(${LIB_NAME} SHARED ${RCRAFT_MAP_SERVER_SRC})
target_include_directories(${LIB_NAME}
        PUBLIC ${OpenCV_INCLUDE_DIRS}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${LIB_NAME} PUBLIC ${OpenCV_LIBS})

# ----------- install/export 추가 -------------
include(GNUInstallDirs)

install(TARGETS ${LIB_NAME}
        EXPORT ${LIB_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}    # .dll
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}    # .lib/.a
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}    # .so/.dll(unix)
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${LIB_NAME}Targets
        FILE ${LIB_NAME}Targets.cmake
        NAMESPACE ${LIB_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${LIB_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME}
)
# ----------- install/export 끝 -------------

add_executable(test src/test/test.cpp)
target_link_libraries(test PUBLIC ${LIB_NAME})