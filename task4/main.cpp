#include <opencv2/opencv.hpp>
#include <pangolin/pangolin.h>

int main(int argc, char** argv){
    // 创建视窗
    pangolin::CreateWindowAndBind("MultiImage", 752, 480);
    // 启动深度测试
    glEnable(GL_DEPTH_TEST);
    // 设置摄像机
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(752, 480, 420, 420, 320, 320, 0.1, 1000),
        pangolin::ModelViewLookAt(-2, 0, -2, 0, 0, 0, pangolin::AxisY)
    );
    // ---------- 创建三个视窗 ---------- //
    pangolin::View& d_cam = pangolin::Display("cam")
        .SetBounds(0., 1., 0., 1., -752/480.)
        .SetHandler(new pangolin::Handler3D(s_cam));
    
    pangolin::View& cv_img_1 = pangolin::Display("image_1")
        .SetBounds(2/3.0f, 1.0f, 0., 1/3.0f, 752/480.)
        .SetLock(pangolin::LockLeft, pangolin::LockTop);
    
    pangolin::View& cv_img_2 = pangolin::Display("image_2")
        .SetBounds(0., 1/3.0f, 2/3.0f, 1.0, 752/480.)
        .SetLock(pangolin::LockRight, pangolin::LockBottom);
    // 创建glTexture容器用于读取图像
    pangolin::GlTexture imgTexture1(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);
    pangolin::GlTexture imgTexture2(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);

    while(!pangolin::ShouldQuit()){
        // 清空颜色和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 启动相机
        d_cam.Activate(s_cam);
        // 添加一个立方体
        glColor3f(1.0f, 1.0f, 1.0f);
        pangolin::glDrawColouredCube();
        // 从文件读取图像
        cv::Mat img1 = cv::imread("../task4/img/img1.png");
        cv::Mat img2 = cv::imread("../task4/img/img2.png");
        // 向GPU装载图像
        imgTexture1.Upload(img1.data, GL_BGR, GL_UNSIGNED_BYTE);
        imgTexture2.Upload(img2.data, GL_BGR, GL_UNSIGNED_BYTE);
        // 显示图像
        cv_img_1.Activate();
        glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
        imgTexture1.RenderToViewportFlipY(); // 需要反转Y轴，否则输出是倒着的
        
        cv_img_2.Activate();
        glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
        imgTexture2.RenderToViewportFlipY();

        pangolin::FinishFrame();
    }

    return 0;
}