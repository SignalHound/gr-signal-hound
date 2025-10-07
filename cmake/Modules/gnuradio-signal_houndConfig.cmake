find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_SIGNAL_HOUND gnuradio-signal_hound)

FIND_PATH(
    GR_SIGNAL_HOUND_INCLUDE_DIRS
    NAMES gnuradio/signal_hound/api.h
    HINTS $ENV{SIGNAL_HOUND_DIR}/include
        ${PC_SIGNAL_HOUND_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_SIGNAL_HOUND_LIBRARIES
    NAMES gnuradio-signal_hound
    HINTS $ENV{SIGNAL_HOUND_DIR}/lib
        ${PC_SIGNAL_HOUND_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-signal_houndTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_SIGNAL_HOUND DEFAULT_MSG GR_SIGNAL_HOUND_LIBRARIES GR_SIGNAL_HOUND_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_SIGNAL_HOUND_LIBRARIES GR_SIGNAL_HOUND_INCLUDE_DIRS)
