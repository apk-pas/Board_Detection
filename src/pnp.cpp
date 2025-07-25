#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
using namespace cv;
using namespace std;

extern Point2f corners[4];

void pnp(Point2f* corners,Mat& tvec,Mat& rvec)
{
    //定义3D世界坐标系中的参考点
    //世界系原点为左上角的六边形
    vector<Point3f> objectPoints;
    objectPoints.emplace_back(0, 0, 0);
    objectPoints.emplace_back(5.5, 0, 0);
    objectPoints.emplace_back(5.5, 5.5, 0);
    objectPoints.emplace_back(0, 5.5, 0);

    //初始化2D图像点与3D世界点的对应关系(不一定一一对应)
    vector<Point2f> imagePoints = {
        corners[0],
        corners[1],
        corners[2],
        corners[3]
    };

    //计算四个点x,y坐标之和
    int a=corners[0].x+corners[0].y;
    int b=corners[1].x+corners[1].y;
    int c=corners[2].x+corners[2].y;
    int d=corners[3].x+corners[3].y;

    //x,y坐标之和最小的点为左上角的六边形
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

    //相机内参矩阵
    Mat cameraMatrix = (Mat_<double>(3, 3) << 
        1036, 0, 640,
        0, 1036, 320,
        0, 0, 1
    );

    //相机畸变系数
    Mat distCoeffs = Mat::zeros(4, 1, CV_64F);

    //使用solvePnP输出旋转向量rvec和平移向量tvec
    bool success = solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);

}