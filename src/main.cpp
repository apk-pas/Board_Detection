#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace cv;
using namespace std;

// 全局变量
Point2f corners[4];
bool isDetected = false;

// 函数声明
Mat origin_Process(Mat img);
void find(Mat edge, Mat img);
void capture(VideoCapture video);
void tests(VideoCapture video);
void pnp(Point2f* corners, Mat& tvec, Mat& rvec);
int open_serial(const char* device, speed_t baudrate);
void send_message(int fd, float x, float y, float z);
bool receive_message(int fd);

int main()
{
    // 打开串口
    int serial_fd = open_serial("/dev/ttyACM0", B115200);
    if (serial_fd == -1) {
        cerr << "串口打开失败，程序退出" << endl;
        return 1;
    }

    // 设置串口为阻塞模式
    int flags = fcntl(serial_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("获取文件状态失败");
        close(serial_fd);
        return 1;
    }
    flags &= ~O_NONBLOCK;  // 清除非阻塞标志
    if (fcntl(serial_fd, F_SETFL, flags) == -1) {
        perror("设置阻塞模式失败");
        close(serial_fd);
        return 1;
    }

    // 打开摄像头
    VideoCapture video(0);
    if (!video.isOpened()) {
        cerr << "无法打开摄像头" << endl;
        close(serial_fd);
        return 1;
    }

    // 创建窗口
    namedWindow("Result", WINDOW_AUTOSIZE);
    namedWindow("Edge", WINDOW_AUTOSIZE);

    // 处理视频帧并发送数据
    Mat frame, edge;
    Mat tvec, rvec;

    while (true) {
        // 读取一帧图像
        video >> frame;
        if (frame.empty()) {
            cerr << "无法获取视频帧" << endl;
            break;
        }

        // 图像处理流程
        edge = origin_Process(frame);
        find(edge, frame); 

        // 检测到目标时进行PNP计算并发送数据
        if (isDetected) {
            pnp(corners, tvec, rvec);
            
            // 提取平移向量，转成内存较小的浮点数
            float x = static_cast<float>(tvec.at<double>(0));
            float y = static_cast<float>(tvec.at<double>(1));
            float z = static_cast<float>(tvec.at<double>(2));

            // 发送数据到串口
            send_message(serial_fd, x, y, z);

            // 接收回传消息
            receive_message(serial_fd);
        }

        // 等待按键退出
        char key = waitKey(30);
        if (key == 27) {  // ESC键
            break;
        } else if (key == 't') {  // 按't'键进入测试模式
            tests(video);
            destroyWindow("Params");  // 退出测试模式后关闭参数窗口
        }
    }

    // 资源释放
    video.release();
    close(serial_fd);
    destroyAllWindows();

    return 0;
}