/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sp_series_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
    namespace signal_hound {
        using output_type = gr_complex;
        sp_series::sptr sp_series::make(double reflevel,
                                        int atten,
                                        double center,
                                        int decimation,
                                        bool swfilter,
                                        double bandwidth,
                                        bool purge) 
        {
            return gnuradio::make_block_sptr<sp_series_impl>(
                reflevel, atten, center, decimation, swfilter, bandwidth, purge);
        }

        void ERROR_CHECK(SpStatus status)
        {
            if(status != spNoError) {
                bool isWarning = status > spNoError;
                std::cout << "** " << (isWarning ? "Warning: " : "Error: ") << spGetErrorString(status)
                << " **" << "\n";
                if(!isWarning) {
                    abort();
                }
            }
        }


        /*
         * The private constructor
         */
        sp_series_impl::sp_series_impl(double reflevel, 
                                       int atten, 
                                       double center, 
                                       int decimation, 
                                       bool swfilter, 
                                       double bandwidth, 
                                       bool purge) : 
            gr::sync_block("sp_series",
            gr::io_signature::make(0, 0, 0),
            gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
            _handle(-1),
            _center(center),
            _reflevel(reflevel),
            _atten(atten),
            _decimation(decimation),
            _bandwidth(bandwidth),
            _purge(purge ? spTrue : spFalse),
            _swfilter(swfilter ? spTrue : spFalse),
            _param_changed(true),
            _buffer(0),
            _len(0) 
        {
            std::cout << "\nAPI Version: " << spGetAPIVersion() << std::endl;

            // Open device
            ERROR_CHECK(spOpenDevice(&_handle));

            int serial;
            ERROR_CHECK(spGetSerialNumber(_handle, &serial));
            std::cout << "Serial Number: "<< serial << std::endl;
        }

        /*
         * Our virtual destructor.
         */
        sp_series_impl::~sp_series_impl() 
        {
            spAbort(_handle);
            spCloseDevice(_handle);
            if (_buffer) {
                delete [] _buffer;
            }
        }

        void sp_series_impl::set_center(double center)
        {
            gr::thread::scoped_lock lock(_mutex);
            _center = center;
            _param_changed = true;
        }

        void sp_series_impl::set_reflevel(double reflevel)
        {
            gr::thread::scoped_lock lock(_mutex);
            _reflevel = reflevel;
            _param_changed = true;
        }

        void sp_series_impl::set_atten(int atten) 
        {
            gr::thread::scoped_lock lock(_mutex);
            _atten = atten;
            _param_changed = true;
        }

        void sp_series_impl::set_decimation(int decimation)
        {
            gr::thread::scoped_lock lock(_mutex);
            _decimation = decimation;
            _param_changed = true;
        }

        void sp_series_impl::set_bandwidth(double bandwidth)
        {
            gr::thread::scoped_lock lock(_mutex);
            _bandwidth = bandwidth;
            _param_changed = true;
        }

        void sp_series_impl::set_swfilter(bool swfilter)
        {
            gr::thread::scoped_lock lock(_mutex);
            _swfilter = swfilter ? spTrue : spFalse;
            _param_changed = true;
        }

        void sp_series_impl::set_purge(bool purge)
        {
            gr::thread::scoped_lock lock(_mutex);
            _purge = purge ? spTrue : spFalse;
        }

        void sp_series_impl::configure()
        {
            gr::thread::scoped_lock lock(_mutex);

            // Configure
            ERROR_CHECK(spSetIQDataType(_handle, spDataType32fc));
            ERROR_CHECK(spSetIQCenterFreq(_handle, _center));
            ERROR_CHECK(spSetIQSampleRate(_handle, _decimation));
            ERROR_CHECK(spSetIQSoftwareFilter(_handle, _swfilter));
            ERROR_CHECK(spSetRefLevel(_handle, _reflevel));
            ERROR_CHECK(spSetAttenuator(_handle, _atten));
            ERROR_CHECK(spSetIQBandwidth(_handle, _bandwidth));

            // Initiate for I/Q streaming
            ERROR_CHECK(spConfigure(_handle, spModeIQStreaming));

            // Get I/Q streaming info
            double sampleRate, actualBandwidth;
            ERROR_CHECK(spGetIQParameters(_handle, &sampleRate, &actualBandwidth));
            std::cout << "\nSample Rate: "<< sampleRate << std::endl;
            std::cout << "Actual Bandwidth: "<< actualBandwidth << std::endl;
        }

        int sp_series_impl::work(int noutput_items,
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
            ERROR_CHECK(spGetIQ(_handle, _buffer, noutput_items, 0, 0, 0, _purge ? spTrue : spFalse, 0, 0));

            // Move data to output array
            for(int i = 0; i < noutput_items; i++) {
                out[i] =  _buffer[i];
            }

            return noutput_items;
        }

    } /* namespace signal_hound */
} /* namespace gr */
