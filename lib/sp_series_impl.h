/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SIGNAL_HOUND_SP_SERIES_IMPL_H
#define INCLUDED_SIGNAL_HOUND_SP_SERIES_IMPL_H

#include <gnuradio/signal_hound/sp_series.h>
#include <gnuradio/signal_hound/sp_api.h>

namespace gr {
    namespace signal_hound {
        class sp_series_impl : public sp_series {
            private:
                int _handle;

                double _center;
                double _reflevel;
                int _atten, _decimation; 
                double _bandwidth; 
                SpBool _purge;
                SpBool _swfilter;

                gr::thread::mutex _mutex;
                bool _param_changed;

                std::complex<float> *_buffer;
                int _len;


            public:
                sp_series_impl(double reflevel,
                               int atten,
                               double center,
                               int decimation,
                               bool swfilter,
                               double bandwidth,
                               bool purge);
                ~sp_series_impl(void);

                void set_center(double center);
                void set_reflevel(double reflevel);
                void set_atten(int atten);
                void set_decimation(int decimation);
                void set_bandwidth(double bandwidth);
                void set_purge(bool purge);
                void set_swfilter(bool swfilter);

                void configure(void);

                // Where all the action really happens
                int work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items);
        };
    } // namespace signal_hound
} // namespace gr
#endif /* INCLUDED_SIGNAL_HOUND_SP_SERIES_IMPL_H */
