#!/usr/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/lib
export PATH=/home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/build/lib:$PATH
export LD_LIBRARY_PATH=/home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-aistx 
