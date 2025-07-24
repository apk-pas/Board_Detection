#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;

void pnp(Point2f* corners,Mat& tvec,Mat& rvec)
{
    vector<Point3f> objectPoints;
    objectPoints.emplace_back(0, 0, 0);
    objectPoints.emplace_back(5.5, 0, 0);
    objectPoints.emplace_back(5.5, 5.5, 0);
    objectPoints.emplace_back(0, 5.5, 0);

    vector<Point2f> imagePoints = {
        corners[0],
        corners[1],
        corners[2],
        corners[3]
    };

    int a=corners[0].x+corners[0].y;
    int b=corners[1].x+corners[1].y;
    int c=corners[2].x+corners[2].y;
    int d=corners[3].x+corners[3].y;

    if(a==MIN(MIN(a,b),MIN(c,d))) {}
    if(d==MIN(MIN(a,b),MIN(c,d)))
    {
        imagePoints = {
            corners[1],
            corners[2],
            corners[3],
            corners[0]
        };
    }

    Mat cameraMatrix = (Mat_<double>(3, 3) << 
        1036, 0, 640,
        0, 1036, 320,
        0, 0, 1
    );

    Mat distCoeffs = Mat::zeros(4, 1, CV_64F);

    bool success = solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);

}