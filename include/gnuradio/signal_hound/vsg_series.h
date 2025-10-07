/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SIGNAL_HOUND_VSG_SERIES_H
#define INCLUDED_SIGNAL_HOUND_VSG_SERIES_H

#include <gnuradio/signal_hound/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace signal_hound {

/*!
 * \brief <+description of block+>
 * \ingroup signal_hound
 *
 */
class SIGNAL_HOUND_API vsg_series : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<vsg_series> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of signal_hound::vsg_series.
     *
     * To avoid accidental use of raw pointers, signal_hound::vsg_series's
     * constructor is in a private implementation
     * class. signal_hound::vsg_series::make is the public interface for
     * creating new instances.
     */
    static sptr make(double center,
                     double samplerate,
                     double level,
                     int ioffset,
                     int qoffset);
    virtual void set_center(double center) = 0;
    virtual void set_samplerate(double samplerate) = 0;
    virtual void set_level(double level) = 0;
    virtual void set_ioffset(int ioffset) = 0;
    virtual void set_qoffset(int qoffset) = 0;
};

} // namespace signal_hound
} // namespace gr

#endif /* INCLUDED_SIGNAL_HOUND_VSG_SERIES_H */
