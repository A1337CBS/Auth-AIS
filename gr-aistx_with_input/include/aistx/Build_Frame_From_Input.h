/* -*- c++ -*- */
/* 
 * Copyright 2020 gr-aistx author.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_AISTX_BUILD_FRAME_FROM_INPUT_H
#define INCLUDED_AISTX_BUILD_FRAME_FROM_INPUT_H

#include <aistx/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace aistx {

    /*!
     * \brief <+description of block+>
     * \ingroup aistx
     *
     */
    class AISTX_API Build_Frame_From_Input : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<Build_Frame_From_Input> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of aistx::Build_Frame_From_Input.
       *
       * To avoid accidental use of raw pointers, aistx::Build_Frame_From_Input's
       * constructor is in a private implementation
       * class. aistx::Build_Frame_From_Input::make is the public interface for
       * creating new instances.
       */
      static sptr make(const char *sentence, bool repeat, bool enable_NRZI, bool enable_input, const std::string& len_tag_key);
    };

  } // namespace aistx
} // namespace gr

#endif /* INCLUDED_AISTX_BUILD_FRAME_FROM_INPUT_H */

