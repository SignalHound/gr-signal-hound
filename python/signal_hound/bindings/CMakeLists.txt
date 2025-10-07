# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT signal_hound_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT signal_hound_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND signal_hound_python_files
    bb_series_python.cc
    sp_series_python.cc
    sm_series_python.cc
    vsg_series_python.cc python_bindings.cc)

gr_pybind_make_oot(signal_hound ../../.. gr::signal_hound "${signal_hound_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET signal_hound_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:signal_hound_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/signal_hound/)

install(
    TARGETS signal_hound_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/signal_hound
    COMPONENT pythonapi)
