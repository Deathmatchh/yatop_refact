#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "license_generator::lccgen" for configuration "RelWithDebInfo"
set_property(TARGET license_generator::lccgen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(license_generator::lccgen PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/lccgen.exe"
  )

list(APPEND _cmake_import_check_targets license_generator::lccgen )
list(APPEND _cmake_import_check_files_for_license_generator::lccgen "${_IMPORT_PREFIX}/bin/lccgen.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)