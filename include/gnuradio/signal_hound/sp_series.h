/* -*- c++ -*- */
/*
 * Copyright 2025 Signal Hound.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SIGNAL_HOUND_SP_SERIES_H
#define INCLUDED_SIGNAL_HOUND_SP_SERIES_H

#include <gnuradio/signal_hound/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace signal_hound {

    /*!
     * \brief <+description of block+>
     * \ingroup signal_hound
     *
     */
    class SIGNAL_HOUND_API sp_series : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<sp_series> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of signal_hound::sp_series.
       *
       * To avoid accidental use of raw pointers, signal_hound::sp_series's
       * constructor is in a private implementation
       * class. signal_hound::sp_series::make is the public interface for
       * creating new instances.
       */
      static sptr make(double reflevel, 
                       int atten, 
                       double center, 
                       int decimation, 
                       bool swfilter, 
                       double bandwidth, 
                       bool purge);
      virtual void set_center(double center) = 0;
      virtual void set_reflevel(double reflevel) = 0;
      virtual void set_atten(int atten) = 0;
      virtual void set_decimation(int decimation) = 0;
      virtual void set_swfilter(bool swfilter) = 0;
      virtual void set_purge(bool purge) = 0;
      virtual void set_bandwidth(double bandwidth) = 0;
    };

  } // namespace signal_hound
} // namespace gr

#endif /* INCLUDED_SIGNAL_HOUND_SP_SERIES_H */
