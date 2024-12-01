#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Check::check" for configuration "Debug"
set_property(TARGET Check::check APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Check::check PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/check.lib"
  )

list(APPEND _cmake_import_check_targets Check::check )
list(APPEND _cmake_import_check_files_for_Check::check "${_IMPORT_PREFIX}/lib/check.lib" )

# Import target "Check::checkShared" for configuration "Debug"
set_property(TARGET Check::checkShared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Check::checkShared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/checkDynamic.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/checkDynamic.dll"
  )

list(APPEND _cmake_import_check_targets Check::checkShared )
list(APPEND _cmake_import_check_files_for_Check::checkShared "${_IMPORT_PREFIX}/lib/checkDynamic.lib" "${_IMPORT_PREFIX}/bin/checkDynamic.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
