#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat origin_Process(Mat img);
int Match_Image(Mat img);

int main()
{
    Mat img = imread("/home/summer/Board_Detection/photo/test.jpg");
    if(img.empty())
    {
        cerr << "img is empty" << endl;
        exit(1);
    }
    Mat edge = origin_Process(img);

    if(Match_Image(edge))
    {
        cout << "good" << endl;
    }
    else
    {
        cout << "bad" << endl;
    }

    waitKey(0);
    return 0;
}