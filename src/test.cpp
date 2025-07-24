#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

void nothing(int, void*) {}

void tests(VideoCapture video)
{
    // 创建参数滑动条窗口
    namedWindow("Params", WINDOW_AUTOSIZE);

    // HSV 滑动条（黄色范围可调）
    int h_min = 24, h_max = 31;
    int s_min = 50, s_max = 255;
    int v_min = 110, v_max = 255;

    // 腐蚀膨胀参数
    int erode_iters = 2;
    int dilate_iters = 2;
    int morph_kernel = 3; 

    // 高斯模糊核大小
    int blur_ksize = 3;

    // Canny 阈值
    int canny_thresh1 = 150;
    int canny_thresh2 = 100;

    //精度比例设置
    int approx_factor = 20;

    // 面积筛选
    int min_area = 200;
    int max_area = 10000;

    // 创建滑动条
    createTrackbar("H Min", "Params", &h_min, 180, nothing);
    createTrackbar("H Max", "Params", &h_max, 180, nothing);
    createTrackbar("S Min", "Params", &s_min, 255, nothing);
    createTrackbar("S Max", "Params", &s_max, 255, nothing);
    createTrackbar("V Min", "Params", &v_min, 255, nothing);
    createTrackbar("V Max", "Params", &v_max, 255, nothing);

    createTrackbar("Erode Iters", "Params", &erode_iters, 10, nothing);
    createTrackbar("Dilate Iters", "Params", &dilate_iters, 10, nothing);
    createTrackbar("Morph Kernel", "Params", &morph_kernel, 21, nothing);

    createTrackbar("Blur KSize", "Params", &blur_ksize, 20, nothing);
    createTrackbar("Canny Thresh1", "Params", &canny_thresh1, 255, nothing);
    createTrackbar("Canny Thresh2", "Params", &canny_thresh2, 255, nothing);

    createTrackbar("Approx x1000", "Params", &approx_factor, 100, nothing);

    createTrackbar("Min Area", "Params", &min_area, 5000, nothing);
    createTrackbar("Max Area", "Params", &max_area, 50000, nothing);

    while (true)
    {
        Mat frame;
        video >> frame;
        if (frame.empty())
            break;

        Mat hsv, mask;

        // 转HSV
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // 保证核大小为奇数且>=3
        if (morph_kernel % 2 == 0) morph_kernel += 1;
        morph_kernel = max(3, morph_kernel);

        // 高斯模糊核大小为奇数且 >=3
        if (blur_ksize % 2 == 0) blur_ksize += 1;
        blur_ksize = max(3, blur_ksize);

        // HSV阈值滤波
        inRange(hsv, Scalar(h_min, s_min, v_min), Scalar(h_max, s_max, v_max), mask);

        // 腐蚀膨胀核
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(morph_kernel, morph_kernel));

        // 腐蚀
        for (int i = 0; i < erode_iters; i++)
            dilate(mask, mask, kernel);

        // 膨胀
        for (int i = 0; i < dilate_iters; i++)
            erode(mask, mask, kernel);

        // 闭开运算去噪
        morphologyEx(mask, mask, MORPH_OPEN, kernel);
        morphologyEx(mask, mask, MORPH_CLOSE, kernel);

        // 高斯模糊
        GaussianBlur(mask, mask, Size(blur_ksize, blur_ksize), 0);

        // Canny 边缘检测
        Mat edge;
        Canny(mask, edge, canny_thresh1, canny_thresh2);

        // 轮廓查找
        vector<vector<Point>> contours;
        findContours(edge, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<Point> allHexPoints;
        Mat result = frame.clone();
        int a = 0;

        //逼近精度设置
        double approx_eps = approx_factor / 1000.0;

        for (size_t i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i]);
            if (area < min_area || area > max_area)
                continue;

            vector<Point> approx;
            approxPolyDP(contours[i], approx, arcLength(contours[i], true) * approx_eps, true);

            drawContours(result, vector<vector<Point>>{approx}, 0, Scalar(255, 0, 0), 4);

            // 判断是否为凹六边形
            if (approx.size() == 6 && !isContourConvex(approx))
            {
                allHexPoints.insert(allHexPoints.end(), approx.begin(), approx.end());
            }
            a++;
        }

        // 如果找到了六边形点，绘制最小外接矩形
        if (!allHexPoints.empty())
        {
            RotatedRect bounding = minAreaRect(allHexPoints);
            Point2f corners[4];
            bounding.points(corners);
            Point2f ctr = bounding.center;

            for (int i = 0; i < 4; i++)
                line(result, corners[i], corners[(i + 1) % 4], Scalar(0, 255, 0), 4);

            circle(result, ctr, 4, Scalar(0, 255, 0), -1);
        }

        // 显示窗口
        imshow("HSV Mask", mask);
        imshow("Edge", edge);
        imshow("Result", result);

        char key = (char)waitKey(30);
        if (key == 27) // ESC键退出
            break;
    }
}
