/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SIGNAL_HOUND_VSG_SERIES_IMPL_H
#define INCLUDED_SIGNAL_HOUND_VSG_SERIES_IMPL_H

#include <gnuradio/signal_hound/vsg_series.h>
#include <gnuradio/signal_hound/vsg_api.h>

namespace gr {
namespace signal_hound {

class vsg_series_impl : public vsg_series
{
private:
    int _handle;

    double _center, _samplerate, _level;
    int _ioffset, _qoffset; 

    gr::thread::mutex _mutex;
    bool _param_changed;

    std::complex<float> *_buffer;
    int _len;

public:
    vsg_series_impl(double center,
                    double samplerate,
                    double level,
                    int ioffset,
                    int qoffset);
    ~vsg_series_impl();

    void set_center(double center);
    void set_samplerate(double samplerate);
    void set_level(double level);
    void set_ioffset(int ioffset);
    void set_qoffset(int qoffset);

    void configure(void);

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace signal_hound
} // namespace gr

#endif /* INCLUDED_SIGNAL_HOUND_VSG_SERIES_IMPL_H */
