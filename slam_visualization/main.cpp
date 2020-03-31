#include <pangolin/pangolin.h>
#include <queue>
#include <stdio.h>
#include <unistd.h>
#include "slam_visualizer.h"
#include <thread>
using namespace std;

SlamVisualizer visualizer(1504, 960);
queue<string> imgFileNames;
queue<ulong> imgTimeStamps;


int main(int argc, char** argv){
    FILE *fp_gt, *fp_img;
    // 请自行修改数据集路径
    fp_gt = fopen("/home/yuntian/dataset/MH_05_difficult/mav0/state_groundtruth_estimate0/data.csv", "r");
    fp_img = fopen("/home/yuntian/dataset/MH_05_difficult/mav0/cam0/data.csv", "r");
    if(fp_gt == nullptr || fp_img == nullptr){
        cout << "failed to open file !\n";
        return -1;
    }
    // =================== 读取图片路径 ====================//
     // 跳过第一行
    char fl_buf[1024];
    fgets(fl_buf, sizeof(fl_buf), fp_img);
  
    while(!feof(fp_img)){
        char filename[23];
        ulong timestamp;
        fscanf(fp_img, "%lu,%s", &timestamp, filename);
        
        imgTimeStamps.push(timestamp);
        imgFileNames.push(string(filename));
    }
    // ===================读取groundtruth =================================== //
    // 跳过第一行
    fgets(fl_buf, sizeof(fl_buf), fp_gt);

    // 初始化视窗
    visualizer.initDraw();
    vector<Eigen::Vector3d> traj;
    while (!feof(fp_gt))
    {
        // 常规操作
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        visualizer.activeAllView();
        // 注册ui回调函数
        visualizer.registerUICallback();
        // 从数据集中读取数据
        // 创建数据寄存器    
        ulong time_stamp(0);
        double px(0.), py(0.), pz(0.);
        double qw(0.), qx(0.), qy(0.), qz(0.);
        double vx(0.), vy(0.), vz(0.);
        double bwx(0.), bwy(0.), bwz(0.), bax(0.), bay(0.), baz(0.);
        fscanf(fp_gt, "%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
            &time_stamp, &px, &py, &pz,
            &qw, &qx, &qy, &qz,
            &vx, &vy, &vz,
            &bwx, &bwy, &bwz,
            &bax, &bay, &baz);

        Eigen::Quaterniond quat(qw, qx, qy, qz); //quat是否要转置？
        Eigen::Vector3d pos(px, py, pz);
        traj.push_back(pos);
        // 显示数据
        visualizer.displayData(pos, quat);
        // 绘制轨迹可视化部分
        visualizer.drawCoordinate();
        visualizer.drawCamWithPose(pos, quat);
        visualizer.drawTraj(traj);
        // 弹出当前时刻之前的图像
        double imu_time, img_time;
        imu_time = (double)time_stamp / 1e9; 
        img_time = (double)imgTimeStamps.front() / 1e9;
        if(imu_time > img_time){
            // cout << imgFileNames.front() << endl;
            imgTimeStamps.pop();
            imgFileNames.pop();
        }
        // cout << "current size of img: " << imgFileNames.size() << endl;
        // 显示图像(由于数据集没有跟踪图像，这里两幅图像显示一样)
        cv::Mat img;
        string img_file = "/home/yuntian/dataset/MH_05_difficult/mav0/cam0/data/" + imgFileNames.front();
        img = cv::imread(img_file, CV_LOAD_IMAGE_COLOR);
        visualizer.displayImg(img, img);
        // 循环与退出判断
        pangolin::FinishFrame();
        
        if(pangolin::ShouldQuit())
            break;

        // 200hz
        // usleep(5000); // 挂起5ms
    }

    return 0;
}

