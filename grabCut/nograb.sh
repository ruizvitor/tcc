#!/bin/bash
# RESIZE=$(python grabmini.py $1)
#python grabmini.py $1
python addmask.py grabcut_output.png $1
#y=${$1%.*}
#../imgProc/./runme $yb.JPG
../imgProc/./align $1b.JPG
