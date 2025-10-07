/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SIGNAL_HOUND_BB_SERIES_IMPL_H
#define INCLUDED_SIGNAL_HOUND_BB_SERIES_IMPL_H

#include <gnuradio/signal_hound/bb_series.h>
#include <gnuradio/signal_hound/bb_api.h>

namespace gr {
    namespace signal_hound {
        class bb_series_impl : public bb_series {
            private:
                int _handle;

                double _center;
                double _reflevel;
                int _decimation;
                double _bandwidth;
                bool _purge;

                gr::thread::mutex _mutex;
                bool _param_changed;

                std::complex<float> *_buffer;
                int _len;


            public:
                bb_series_impl(double center,
                               double reflevel,
                               int decimation,
                               double bandwidth,
                               bool purge);
                ~bb_series_impl(void);

                void set_center(double center);
                void set_reflevel(double reflevel);
                void set_decimation(int decimation);
                void set_bandwidth(double bandwidth);
                void set_purge(bool purge);

                void configure(void);

                // Where all the action really happens
                int work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items);
        };
    } // namespace signal_hound
} // namespace gr
#endif /* INCLUDED_SIGNAL_HOUND_BB_SERIES_IMPL_H */
