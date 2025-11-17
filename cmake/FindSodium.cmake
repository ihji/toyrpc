# cmake/FindSodium.cmake
# Finds libsodium and defines Sodium::Sodium imported target.

set(_SODIUM_HINTS
  $ENV{SODIUM_ROOT}
  /opt/homebrew
  /usr/local
  /opt/local
  /usr
)

find_path(SODIUM_INCLUDE_DIR
  NAMES sodium.h
  HINTS ${_SODIUM_HINTS}
  PATH_SUFFIXES include
)

find_library(SODIUM_LIBRARY
  NAMES sodium
  HINTS ${_SODIUM_HINTS}
  PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sodium
  REQUIRED_VARS SODIUM_LIBRARY SODIUM_INCLUDE_DIR
)

if(Sodium_FOUND AND NOT TARGET Sodium::Sodium)
  add_library(Sodium::Sodium UNKNOWN IMPORTED)
  set_target_properties(Sodium::Sodium PROPERTIES
    IMPORTED_LOCATION "${SODIUM_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SODIUM_INCLUDE_DIR}"
  )
endif()

