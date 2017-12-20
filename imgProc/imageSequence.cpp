#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    VideoCapture cap(argv[1]); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    // namedWindow("edges",1);
    int count=0;
    for(;;)
    {
        count+=1;
        Mat frame;
        cap >> frame; // get a new frame from camera
        if(frame.rows==0){
          break;
        }
        // cvtColor(frame, edges, COLOR_BGR2GRAY);
        // GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        // Canny(edges, edges, 0, 30, 3);
        string str="frames/frame"+to_string(count)+".jpg";
        cout << str << endl;
        imwrite(str, frame);
        // imshow("edges", edges);
        // if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
