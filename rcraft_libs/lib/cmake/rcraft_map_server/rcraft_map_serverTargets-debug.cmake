#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "rcraft_map_server::rcraft_map_server" for configuration "Debug"
set_property(TARGET rcraft_map_server::rcraft_map_server APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(rcraft_map_server::rcraft_map_server PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/librcraft_map_server.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/librcraft_map_server.dll"
  )

list(APPEND _cmake_import_check_targets rcraft_map_server::rcraft_map_server )
list(APPEND _cmake_import_check_files_for_rcraft_map_server::rcraft_map_server "${_IMPORT_PREFIX}/lib/librcraft_map_server.dll.a" "${_IMPORT_PREFIX}/bin/librcraft_map_server.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
