cd ..;
cp initial/joel7H3_pospose.txt poseinput.txt;
primusrun ./runme -f initial/joel7H3_pospose.txt -p ../profeta/jo7pros.JPG -m ../profeta/myjoelProcessed.obj -h 1 -i;
cd tests;
