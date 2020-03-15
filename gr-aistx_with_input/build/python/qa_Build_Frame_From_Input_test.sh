#!/usr/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/python
export PATH=/home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/build/python:$PATH
export LD_LIBRARY_PATH=/home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/build/swig:$PYTHONPATH
/usr/bin/python2 /home/ahmad/Documents/GRC/Implementation/Code/AIS_Security_Protocol/gr-aistx_with_input/python/qa_Build_Frame_From_Input.py 
