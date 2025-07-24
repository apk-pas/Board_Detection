# 树莓派使用流程及注意点

## 一、前期准备
1. **格式化SD卡**  
   使用 `SD Card Formatter` 工具格式化SD卡

2. **下载必要工具**  
   - XShell（用于SSH连接）  
   - RealVNC-Viewer（用于VNC远程桌面连接）

3. **下载系统镜像**  
   访问 [Ubuntu 22.04.5 镜像地址](https://cdimage.ubuntu.com/releases/22.04.5/release/)，下载 `ubuntu-22.04.5-preinstalled-server-arm64+raspi.img.xz`

4. **烧写系统到SD卡**  
   使用 `Raspberry Pi Imager` 工具配置并烧写下载的镜像到SD卡

5. **配置网络（电脑热点）**  
   修改 `network-config` 文件（使用电脑发出热点，便于查看IP地址


## 二、系统初始化
1. **启动树莓派并连接**  
   将SD卡插入树莓派，通电启动后，通过 XShell 连接（需知道树莓派IP地址）

2. **更换软件源（解决清华源可能存在的问题）**  
   
   - 编辑源配置文件：  
     ```bash
     sudo nano /etc/apt/sources.list
     ```
   - 将原有内容注释（在每行前加 `#`），添加阿里云源：  
     ```bash
     deb https://mirrors.aliyun.com/ubuntu-ports/ jammy main restricted universe multiverse
     # deb-src https://mirrors.aliyun.com/ubuntu-ports/ jammy main restricted universe multiverse
     deb https://mirrors.aliyun.com/ubuntu-ports/ jammy-updates main restricted universe multiverse
     # deb-src https://mirrors.aliyun.com/ubuntu-ports/ jammy-updates main restricted universe multiverse
     deb https://mirrors.aliyun.com/ubuntu-ports/ jammy-backports main restricted universe multiverse
     # deb-src https://mirrors.aliyun.com/ubuntu-ports/ jammy-backports main restricted universe multiverse
     deb https://mirrors.aliyun.com/ubuntu-ports/ jammy-security main restricted universe multiverse
     # deb-src https://mirrors.aliyun.com/ubuntu-ports/ jammy-security main restricted universe multiverse
     ```
   - 保存退出：按 `Ctrl+O` → 回车确认 → `Ctrl+X`。
   - 更新软件包列表和系统：  
     ```bash
     sudo apt update
     sudo apt upgrade -y
     ```


## 三、基础工具安装
1. **安装 raspi-config**  
   ```bash
   sudo apt install whiptail parted lua5.1 alsa-utils psmisc -y
   sudo wget https://archive.raspberrypi.org/debian/pool/main/r/raspi-config/raspi-config_20220907_all.deb
   sudo dpkg -i raspi-config_20220907_all.deb
   ```

2. **安装桌面环境**  
   ```bash
   sudo apt install ubuntu-desktop -y
   ```

3. **安装远程桌面服务**  
   ```bash
   sudo apt install xrdp -y
   ```


## 四、VNC 配置
1. **下载 VNC Server安装包**  
   
   ```bash
   sudo wget https://downloads.realvnc.com/download/file/vnc.files/VNC-Server-7.14.0-Linux-ARM64.deb
   ```
   
2. **解决安装可能出现的错误**  
   - 若出现 `dpkg error`：  
     ```bash
     sudo apt --fix-broken install
     ```
   - 若出现 `cache lock`（缓存锁）问题：  
     ```bash
     sudo rm /var/lib/apt/lists/lock
     sudo rm /var/cache/apt/archives/lock
     sudo rm /var/lib/dpkg/lock*
     ```
   - 若安装桌面时卡住，重启后执行：  
     ```bash
     sudo dpkg --configure -a
     ```

3. **安装 VNC Server**  
   
   ```bash
   sudo dpkg -i VNC-Server-7.14.0-Linux-ARM64.deb
   ```
   
4. **配置虚拟桌面分辨率**  
   
   - 安装虚拟显示驱动：  
     ```bash
     sudo apt install xserver-xorg-video-dummy
     ```
   - 创建并编辑配置文件：  
     ```bash
     sudo touch /usr/share/X11/xorg.conf.d/xorg.conf
     cd /usr/share/X11/xorg.conf.d
     sudo nano xorg.conf
     ```
   - 写入以下内容（支持1920x1080等分辨率）：  
     ```
     Section "Device"
         Identifier  "Configured Video Device"
         Driver      "dummy"
         VideoRam 256000
     EndSection
     Section "Monitor"
         Identifier  "Configured Monitor"
         HorizSync 5.0 - 1000.0
         VertRefresh 5.0 - 200.0
         ModeLine "1920x1080" 148.50 1920 2448 2492 2640 1080 1084 1089 1125 +Hsync +Vsync
     EndSection
     Section "Screen"
         Identifier  "Default Screen"
         Monitor     "Configured Monitor"
         Device      "Configured Video Device"
         DefaultDepth 24
         SubSection "Display"
         Depth 24
         Modes "1920x1080" "1440x900" "1280x800" "1024x768"
         EndSubSection
     EndSection
     ```
   - 保存退出（`Ctrl+O` → 回车 → `Ctrl+X`）
   
5. **启用 VNC 服务**  
   ```bash
   cd ~
   sudo raspi-config
   ```
   在菜单中用方向键和Enter进行选择：`Interface Options` → `VNC` → 启用VNC

6. **重启树莓派**  
   
   ```bash
   sudo reboot
   ```


## 五、远程连接与后续配置
1. **使用 realVNC-Viewer 连接**  
   打开 realVNC-Viewer，输入树莓派IP地址，按提示完成连接。

2. **修改网络配置（如需配置固定IP地址）**  
   编辑网络配置文件：  
   ```bash
   sudo nano /etc/netplan/50-cloud-init.yaml
   ```
   （修改后需执行 `sudo netplan apply` 或重启生效）