# 使用 Git 向 GitHub 提交代码以保持进度同步

## 一、前期准备
### 1. 确保 Git和ssh 已安装
首先确认本地环境已安装 Git（`sudo apt install git`）

确认已安装ssh（`sudo apt install openssh-server`）

### 2. 初始化 Git 配置
设置全局用户名和邮箱（用于标识提交者身份）：
```bash
git init
git config --global user.name "<your account username>"  # 替换为你的用户名
git config --global user.email "<your email address>"    # 替换为你的注册邮箱
```


## 二、配置 SSH 密钥（实现免密连接）
### 1. 生成 SSH 密钥对
在终端执行以下命令生成密钥（一路回车默认配置即可，也可设置密码增强安全性）：
```bash
ssh-keygen -t rsa -C "<your email address>"  # 邮箱需与上面配置的一致
```

### 2. 获取公钥内容
生成后，查看并复制公钥内容：
```bash
cat ~/.ssh/id_rsa.pub
```

### 3. 配置 GitHub的ssh key
- 登录 GitHub，进入个人设置（Settings）→ SSH and GPG keys → New SSH key
- 将复制的公钥内容粘贴到文本框，点击 Add SSH key

### 4. 验证连接
执行以下命令验证是否连接成功（出现 “Hi [用户名]!” 提示即成功）：
```bash
ssh -T git@github.com
```

## 三、提交代码到 GitHub

### 1. 暂存与提交本地修改
```bash
git add .  # 暂存所有修改
git commit -m "<your commit>"  # 提交暂存的修改，替换为提交说明
```

### 2. 关联远程仓库
将本地仓库与 GitHub 远程仓库关联（`connection name` 可自定义，如 `origin`）：
```bash
git remote add <connection name> <your github ssh address>  # SSH地址格式：git@github.com:用户名/仓库名.git
```

### 3. 推送代码到远程分支
将本地提交推送到远程指定分支（首次推送加 `-u` 关联分支，后续可直接用 `git push`）：
```bash
git push -u <connection name> <branch name>  # 替换为关联名和目标分支名,拉取则改为pull
```

本项目中：

```bash
git pull six main # 先同步项目
git add .
git commit -m "message" # 提交本地修改
git push six main # 推送本地内容
```

## 四、常见操作与问题解决

### 1. 改名（如需）
若需要修改本地分支名称：
```bash
git branch -m <old_branch_name> <new_branch_name>
```

若需要修改连接名称：

```bash
git remote rename <old_connection_name> <new_connection_name>
```

### 2. 解决推送失败（本地库落后于远程库）

当提示“git pull ..”时，若本地与远程分支无关联历史，可使用以下命令拉取并合并：
```bash
git pull <connection name> <branch name> --allow-unrelated-histories
```
（合并后若有冲突，需手动解决冲突文件，再通过 `git add` 和 `git commit` 提交，之后重新推送）