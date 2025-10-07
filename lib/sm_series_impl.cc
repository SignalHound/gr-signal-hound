/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sm_series_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
    namespace signal_hound {
        using output_type = gr_complex;
        sm_series::sptr sm_series::make(double center, 
                                        double reflevel, 
                                        int atten, 
                                        int decimation, 
                                        bool swfilter, 
                                        bool purge, 
                                        double bandwidth) 
        {
            return gnuradio::make_block_sptr<sm_series_impl>(
                center, reflevel, atten, decimation, swfilter, purge, bandwidth);
        }

        void ERROR_CHECK(const char* call, SmStatus status)
        {
            if(status != smNoError) {
                bool isWarning = status > smNoError;
                std::cout << "** " << (isWarning ? "Warning: (" : "Error: (") << call << ")" << smGetErrorString(status)
                << " **" << "\n";
                if(!isWarning) {
                    abort();
                }
            }
        }


        /*
         * The private constructor
         */
        sm_series_impl::sm_series_impl(double center, 
                                       double reflevel, 
                                       int atten, 
                                       int decimation, 
                                       bool swfilter, 
                                       bool purge, 
                                       double bandwidth) : 
            gr::sync_block("sm_series", 
                           gr::io_signature::make(0, 0, 0),
                           gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
            _handle(-1),
            _center(center),
            _reflevel(reflevel),
            _atten(atten),
            _decimation(decimation),
            _bandwidth(bandwidth),
            _purge(purge ? smTrue : smFalse),
            _swfilter(swfilter ? smTrue : smFalse),
            _param_changed(true),
            _buffer(0),
            _len(0) 
        {
            std::cout << "\nAPI Version: " << smGetAPIVersion() << std::endl;

            // Open device
            ERROR_CHECK("smOpenDevice", smOpenDevice(&_handle));

            int serial;
            SmDeviceType dtype;
            ERROR_CHECK("smGetDeviceInfo", smGetDeviceInfo(_handle, &dtype, &serial));
            std::cout << "Serial Number: "<< serial << std::endl;
        }

        /*
         * Our virtual destructor.
         */
        sm_series_impl::~sm_series_impl()
        {
            smAbort(_handle);
            smCloseDevice(_handle);
            if (_buffer) delete [] _buffer;
        }


        void sm_series_impl::set_center(double center)
        {
            gr::thread::scoped_lock lock(_mutex);
            _center = center;
            _param_changed = true;
        }

        void sm_series_impl::set_reflevel(double reflevel)
        {
            gr::thread::scoped_lock lock(_mutex);
            _reflevel = reflevel;
            _param_changed = true;
        }

        void sm_series_impl::set_atten(int atten)
        {
            gr::thread::scoped_lock lock(_mutex);
            _atten = atten;
            _param_changed = true;
        }

        void sm_series_impl::set_decimation(int decimation)
        {
            gr::thread::scoped_lock lock(_mutex);
            _decimation = decimation;
            _param_changed = true;
        }

        void sm_series_impl::set_bandwidth(double bandwidth)
        {
            gr::thread::scoped_lock lock(_mutex);
            _bandwidth = bandwidth;
            _param_changed = true;
        }

        void sm_series_impl::set_swfilter(bool swfilter)
        {
            gr::thread::scoped_lock lock(_mutex);
            _swfilter = swfilter ? smTrue : smFalse;
            _param_changed = true;
        }

        void sm_series_impl::set_purge(bool purge)
        {
            gr::thread::scoped_lock lock(_mutex);
            _purge = purge ? smTrue : smFalse;
        }

        void sm_series_impl::configure()
        {
            gr::thread::scoped_lock lock(_mutex);

            // Configure
            ERROR_CHECK("smSetIQDataType", smSetIQDataType(_handle, smDataType32fc));
            ERROR_CHECK("smSetIQCenterFreq", smSetIQCenterFreq(_handle, _center));
            ERROR_CHECK("smSetIQSampleRate", smSetIQSampleRate(_handle, _decimation));
            ERROR_CHECK("smSetRefLevel", smSetRefLevel(_handle, _reflevel));
            ERROR_CHECK("smSetAttenuator", smSetAttenuator(_handle, _atten));
            ERROR_CHECK("smSetIQBandwidth", smSetIQBandwidth(_handle, _swfilter, _bandwidth));

            // Initiate for I/Q streaming
            ERROR_CHECK("smConfigure", smConfigure(_handle, smModeIQStreaming));

            // Get I/Q streaming info
            double sampleRate, actualBandwidth;
            ERROR_CHECK("smGetIQParameters", smGetIQParameters(_handle, &sampleRate, &actualBandwidth));
            std::cout << "\nSample Rate: "<< sampleRate << std::endl;
            std::cout << "Actual Bandwidth: "<< actualBandwidth << std::endl;
        }

        int sm_series_impl::work(int noutput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items) 
        {
            auto out = static_cast<output_type*>(output_items[0]);

            // Initiate new configuration if necessary
            if(_param_changed) {
                configure();
                _param_changed = false;
            }

            // Allocate memory if necessary
            if(!_buffer || noutput_items != _len) {
                if(_buffer) delete [] _buffer;
                _buffer = new std::complex<float>[noutput_items];
                _len = noutput_items;
            }

            // Get I/Q
            ERROR_CHECK("smGetIQ", smGetIQ(_handle, _buffer, noutput_items, 0, 0, 0, _purge ? smTrue : smFalse, 0, 0));

            // Move data to output array
            for(int i = 0; i < noutput_items; i++) {
                out[i] =  _buffer[i];
            }

            return noutput_items;
        }

    } /* namespace signal_hound */
} /* namespace gr */
