#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void origin_Process(Mat img);

int main()
{
    Mat img = imread("/home/summer/Board_Detection/photo/example.jpg");
    if(img.empty())
    {
        cerr << "img is empty" << endl;
        exit(1);
    }
    origin_Process(img);

    waitKey(0);
    return 0;
}