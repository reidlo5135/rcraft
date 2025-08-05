#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "rcraft_path_planner::rcraft_path_planner" for configuration "Debug"
set_property(TARGET rcraft_path_planner::rcraft_path_planner APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(rcraft_path_planner::rcraft_path_planner PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/librcraft_path_planner.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/librcraft_path_planner.dll"
  )

list(APPEND _cmake_import_check_targets rcraft_path_planner::rcraft_path_planner )
list(APPEND _cmake_import_check_files_for_rcraft_path_planner::rcraft_path_planner "${_IMPORT_PREFIX}/lib/librcraft_path_planner.dll.a" "${_IMPORT_PREFIX}/bin/librcraft_path_planner.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
