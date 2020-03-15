/* -*- c++ -*- */

#define AISTX_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "aistx_swig_doc.i"

%{
#include "aistx/Build_Frame_From_Input.h"
%}


%include "aistx/Build_Frame_From_Input.h"
GR_SWIG_BLOCK_MAGIC2(aistx, Build_Frame_From_Input);
