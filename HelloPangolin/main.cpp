#include <pangolin/pangolin.h>

int main( int /*argc*/, char** /*argv*/ )
{
    // 创建名称为“Main”的GUI窗口，尺寸为640×640
    pangolin::CreateWindowAndBind("Main",640,480);
    // 启动深度测试(即关闭透视，只会看到最前面的物体，否则将会看到后面的物体)
    glEnable(GL_DEPTH_TEST);

    // 创建一个观察相机视图
    // ProjectMatrix(int h, int w, int fu, int fv, int cu, int cv, int znear, int zfar) 
    //      参数依次为观察相机的图像高度、宽度、4个内参以及最近和最远视距
    // ModelViewLookAt(double x, double y, double z,double lx, double ly, double lz, AxisDirection Up)
    //      参数一次为相机所在的位置，以及相机所看的视点位置(一般会设置在原点)
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(640,480,420,420,320,320,0.2,100),
        pangolin::ModelViewLookAt(2,0,2, 0,0,0, pangolin::AxisY)
    );

    // 创建交互视窗
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

        // Swap frames and Process Events
        pangolin::FinishFrame();
    }
    
    return 0;
}
