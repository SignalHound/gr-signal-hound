/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "vsg_series_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace signal_hound {

using input_type = gr_complex;
vsg_series::sptr vsg_series::make(double center,
                                  double samplerate,
                                  double level,
                                  int ioffset,
                                  int qoffset)
{
    return gnuradio::make_block_sptr<vsg_series_impl>(
        center, samplerate, level, ioffset, qoffset);
}

void ERROR_CHECK(const char* call, VsgStatus status)
{
    if(status != vsgNoError) {
        bool isWarning = status > vsgNoError;
        std::cout << "** VSG " << (isWarning ? "Warning: (" : "Error: (") << call << ")" << vsgGetErrorString(status)
        << " **" << "\n";
        if(!isWarning) {
            abort();
        }
    }
}


/*
 * The private constructor
 */
vsg_series_impl::vsg_series_impl(double center,
                                 double samplerate,
                                 double level,
                                 int ioffset,
                                 int qoffset) : 
    gr::sync_block("vsg_series",
    gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
    gr::io_signature::make(0, 0, 0)),
    _handle(-1),
    _center(center),
    _samplerate(samplerate),
    _level(level),
    _ioffset(ioffset),
    _qoffset(qoffset),
    _param_changed(true),
    _buffer(0),
    _len(0) 
{
    std::cout << "\nAPI Version: " << vsgGetAPIVersion() << std::endl;

    // Open device
    ERROR_CHECK("vsgOpenDevice", vsgOpenDevice(&_handle));

    int serial;
    ERROR_CHECK("vsgGetSerialNumber", vsgGetSerialNumber(_handle, &serial));
    std::cout << "Serial Number: "<< serial << std::endl;
}

void vsg_series_impl::configure() 
{
    gr::thread::scoped_lock lock(_mutex);

    // Configure
    ERROR_CHECK("vsgSetFrequency", vsgSetFrequency(_handle, _center));
    ERROR_CHECK("vsgSetSampleRate", vsgSetSampleRate(_handle, _samplerate));
    ERROR_CHECK("vsgSetLevel", vsgSetLevel(_handle, _level));
    ERROR_CHECK("vsgSetIQOffset", vsgSetIQOffset(_handle, (int16_t)_ioffset, (int16_t)_qoffset));

    // Get I/Q info
    double aFreq, aSamp, aLeve;
    int16_t aiOff, aqOff;
    ERROR_CHECK("vsgGetFrequency", vsgGetFrequency(_handle, &aFreq));
    ERROR_CHECK("vsgGetSampleRate", vsgGetSampleRate(_handle, &aSamp));
    ERROR_CHECK("vsgGetLevel", vsgGetLevel(_handle, &aLeve));
    ERROR_CHECK("vsgGetIQOffset", vsgGetIQOffset(_handle, &aiOff, &aqOff));
    std::cout << "\nFrequency: "<< aFreq << std::endl;
    std::cout << "SampleRate: "<< aSamp << std::endl;
    std::cout << "Level: "<< aLeve << std::endl;
    std::cout << "I Offset: "<< aiOff << std::endl;
    std::cout << "Q Offset: "<< aqOff << std::endl;
}

/*
 * Our virtual destructor.
 */
vsg_series_impl::~vsg_series_impl()
{
    vsgAbort(_handle);
    vsgCloseDevice(_handle);
    if (_buffer) {
        delete [] _buffer;
    }
}

void vsg_series_impl::set_center(double center)
{
    gr::thread::scoped_lock lock(_mutex);
    _center = center;
    _param_changed = true;
}

void vsg_series_impl::set_level(double level)
{
    gr::thread::scoped_lock lock(_mutex);
    _level = level;
    _param_changed = true;
}

void vsg_series_impl::set_samplerate(double samplerate)
{
    gr::thread::scoped_lock lock(_mutex);
    _samplerate = samplerate;
    _param_changed = true;
}


void vsg_series_impl::set_ioffset(int ioffset)
{
    gr::thread::scoped_lock lock(_mutex);
    _ioffset = ioffset;
    _param_changed = true;
}

void vsg_series_impl::set_qoffset(int qoffset)
{
    gr::thread::scoped_lock lock(_mutex);
    _qoffset = qoffset;
    _param_changed = true;
}


int vsg_series_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);

    // Initiate new configuration if necessary
    if(_param_changed) {
        configure();
        _param_changed = false;
    }
    vsgSubmitIQ(_handle, (float*)in, noutput_items);
    vsgFlush(_handle);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace signal_hound */
} /* namespace gr */
