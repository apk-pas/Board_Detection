#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// 打开串口
int open_serial(const char* device, speed_t baudrate) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("无法打开串口");
        return -1;
    }

    // 配置串口
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd, &tty) != 0) {
        perror("获取串口属性失败");
        close(fd);
        return -1;
    }

    // 设置波特率
    cfsetospeed(&tty, 115200);
    cfsetispeed(&tty, 115200);

    // 配置数据位:8位数据,无校验,1位停止位
    tty.c_cflag &= ~PARENB;    // 无校验
    tty.c_cflag &= ~CSTOPB;    // 1位停止位
    tty.c_cflag &= ~CSIZE;     // 清除数据位设置
    tty.c_cflag |= CS8;        // 8位数据
    tty.c_cflag &= ~CRTSCTS;   // 禁用硬件流控
    tty.c_cflag |= CREAD | CLOCAL;  // 启用接收,忽略调制解调器控制线

    // 应用配置
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("配置串口属性失败");
        close(fd);
        return -1;
    }

    return fd;
}

// 发送消息
void send_message(int fd, float x, float y, float z) {
    // 格式化消息(101_x_y_z_1,保留2位小数)
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "101_%.2f_%.2f_%.2f_1\n", x, y, z);
    if (len <= 0 || len >= sizeof(buffer)) {
        std::cerr << "消息格式化失败" << std::endl;
        return;
    }

    // 发送消息
    ssize_t bytes_sent = write(fd, buffer, len);
    if (bytes_sent != len) {
        perror("发送消息失败");
    } else {
        std::cout << "发送: " << buffer;
    }
}

// 接收STM32回传消息
bool receive_message(int fd) {
    char buffer[64] = {0};
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        return false;
    }

    buffer[bytes_read] = '\0';  // 添加字符串结束符
    std::cout << "收到回传消息: " << buffer;  // 直接打印收到的内容

    return true;
}
