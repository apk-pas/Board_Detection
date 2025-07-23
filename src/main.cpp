#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat origin_Process(Mat img);
int Match_Image(Mat img);
void find(Mat edge,Mat img);
void capture(VideoCapture video);
void tests(VideoCapture video);

int main()
{
    VideoCapture video(0);
    //capture(video);
    tests(video);

    return 0;
}