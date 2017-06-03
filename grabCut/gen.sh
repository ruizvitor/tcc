#!/bin/bash
python grabmini.py $1
python test.py grabcut_output.png $1
../imgProc/./runme $1b.JPG
