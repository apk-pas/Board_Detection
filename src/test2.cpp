#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

// 全局变量（滑动条用来控制参数）
int minArea = 10000;
int morphIter = 1;
int cannyThreshold1 = 100;
int cannyThreshold2 = 200;
int blurSize = 5;
int hsvLowH = 20, hsvHighH = 32;
int hsvLowS = 80, hsvHighS = 255;
int hsvLowV = 40, hsvHighV = 255;

// 创建滑动条的回调函数
void on_trackbar(int, void*) {}

// 图像预处理：HSV提取黄色区域、腐蚀膨胀、Canny边缘检测
Mat origin_Process2(Mat img)
{
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    // 提取黄色区域
    inRange(hsv, Scalar(hsvLowH, hsvLowS, hsvLowV), Scalar(hsvHighH, hsvHighS, hsvHighV), hsv);
    
    // 高斯模糊
    GaussianBlur(hsv, hsv, Size(blurSize, blurSize), 0);

    // 腐蚀膨胀去噪
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    dilate(hsv, hsv, kernel, Point(-1, -1), morphIter);
    erode(hsv, hsv, kernel, Point(-1, -1), morphIter);
    morphologyEx(hsv, hsv, MORPH_CLOSE, kernel);
    morphologyEx(hsv, hsv, MORPH_OPEN, kernel);

    // Canny边缘检测
    Mat edge;
    Canny(hsv, edge, cannyThreshold1, cannyThreshold2);
    return edge;
}

// 处理每个轮廓区域，提取六边形质心并连接
void findContoursAndProcess(Mat edge, Mat img)
{
    vector<vector<Point>> contours;
    findContours(edge, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<Point> allHexCentroids;

    // 遍历每个轮廓
    for (size_t i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > minArea)
        {
            // 获取轮廓的外接矩形
            Rect boundingBox = boundingRect(contours[i]);
            Mat roi = img(boundingBox);  // 裁剪出该区域

            // 对轮廓进行近似拟合，检测六边形
            vector<Point> approx;
            approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.02, true);

            if (approx.size() == 6 && !isContourConvex(approx))  // 检查是否为非凸六边形
            {
                // 计算六边形的质心
                Moments m = moments(approx);
                Point centroid(m.m10 / m.m00, m.m01 / m.m00);
                allHexCentroids.push_back(centroid);
            }
        }
    }

    // 如果找到至少四个六边形
    if (allHexCentroids.size() >= 4)
    {
        // 连接质心，绘制四边形
        for (size_t i = 0; i < allHexCentroids.size() - 1; i++)
        {
            line(img, allHexCentroids[i], allHexCentroids[i + 1], Scalar(0, 255, 0), 2);
        }
        line(img, allHexCentroids[allHexCentroids.size() - 1], allHexCentroids[0], Scalar(0, 255, 0), 2);

        // 显示每个质心
        for (size_t i = 0; i < allHexCentroids.size(); i++)
        {
            circle(img, allHexCentroids[i], 5, Scalar(0, 0, 255), -1);
        }

        // 输出坐标
        for (size_t i = 0; i < allHexCentroids.size(); i++)
        {
            cout << "Hexagon " << i + 1 << " Centroid: (" << allHexCentroids[i].x << ", " << allHexCentroids[i].y << ")" << endl;
        }
    }

    imshow("Processed Result", img);
}

// 统一封装的图像处理函数
void processImage2(Mat image)
{

    // 读取图像
    Mat img = image;

    if (img.empty())
    {
        cout << "Image not found!" << endl;
        return;
    }

    // 创建窗口
    namedWindow("Processed Result", WINDOW_NORMAL);

    // 创建滑动条
    createTrackbar("Min Area", "Processed Result", &minArea, 50000, on_trackbar);
    createTrackbar("Morph Iter", "Processed Result", &morphIter, 10, on_trackbar);
    createTrackbar("Canny Threshold1", "Processed Result", &cannyThreshold1, 255, on_trackbar);
    createTrackbar("Canny Threshold2", "Processed Result", &cannyThreshold2, 255, on_trackbar);
    createTrackbar("Blur Size", "Processed Result", &blurSize, 21, on_trackbar);
    createTrackbar("HSV Low H", "Processed Result", &hsvLowH, 180, on_trackbar);
    createTrackbar("HSV High H", "Processed Result", &hsvHighH, 180, on_trackbar);
    createTrackbar("HSV Low S", "Processed Result", &hsvLowS, 255, on_trackbar);
    createTrackbar("HSV High S", "Processed Result", &hsvHighS, 255, on_trackbar);
    createTrackbar("HSV Low V", "Processed Result", &hsvLowV, 255, on_trackbar);
    createTrackbar("HSV High V", "Processed Result", &hsvHighV, 255, on_trackbar);

    // 预处理图像并提取边缘
    Mat edge = origin_Process2(img);

    // 查找轮廓并处理
    findContoursAndProcess(edge, img);
}

