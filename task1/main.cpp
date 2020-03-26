#include <pangolin/pangolin.h>

int main( int /*argc*/, char** /*argv*/ )
{
    // 创建名称为“Main”的GUI窗口，尺寸为640×640
    pangolin::CreateWindowAndBind("Main",640,480);
    // 启动深度测试
    glEnable(GL_DEPTH_TEST);

    // 创建一个观察相机视图
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(640,480,420,420,320,320,0.2,100),
        pangolin::ModelViewLookAt(2,0,2, 0,0,0, pangolin::AxisY)
    );

    // 创建交互视图
    pangolin::Handler3D handler(s_cam); //交互相机视图句柄
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(&handler);

    while( !pangolin::ShouldQuit() )
    {
        // 清空颜色和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);

        // 在原点绘制一个立方体
        pangolin::glDrawColouredCube();
        // 绘制坐标系
       	glLineWidth(3);
        glBegin ( GL_LINES );
	    glColor3f ( 0.8f,0.f,0.f );
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( 0,-1,-1 );
	    glColor3f( 0.f,0.8f,0.f);
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( -1,0,-1 );
	    glColor3f( 0.2f,0.2f,1.f);
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( -1,-1,0 );
	    glEnd();

        // 运行帧循环以推进窗口事件
        pangolin::FinishFrame();
    }
    
    return 0;
}
