#include "slam_visualizer.h"

void SlamVisualizer::initDraw(){
    pangolin::CreateWindowAndBind("camera_pose", WIN_WIDTH_, WIN_HEIGHT_);
    glEnable(GL_DEPTH_TEST);

    s_cam_ = pangolin::OpenGlRenderState(
        pangolin::ProjectionMatrix(WIN_WIDTH_, WIN_HEIGHT_, 420, 420, 320, 240, 0.1, 1000),
        pangolin::ModelViewLookAt(5, -3, 5, 0, 0, 0, pangolin::AxisZ)
    );
    int PANEL_WIDTH = WIN_WIDTH_ / 4;
    int PANEL_HEIGHT = WIN_HEIGHT_ / 4;
    // 轨迹显示窗口
    d_cam_ = pangolin::CreateDisplay()
        .SetBounds(0., 1., pangolin::Attach::Pix(PANEL_WIDTH), 1., -(float)WIN_WIDTH_/ (float)WIN_HEIGHT_)
        .SetHandler(new pangolin::Handler3D(s_cam_));
    // 控制面板
    pangolin::CreatePanel("ui")
        .SetBounds(pangolin::Attach::Pix(3.0f *PANEL_HEIGHT), 1., 0., pangolin::Attach::Pix(PANEL_WIDTH), (float)WIN_WIDTH_/ (float)WIN_HEIGHT_);
    
    ui_set_.clear();
    pangolin::Var<bool> show_cam("ui.show_cam", true, true);
    ui_set_.push_back(show_cam); 
    pangolin::Var<bool> show_traj("ui.show_traj", true, true);
    ui_set_.push_back(show_traj); 
    pangolin::Var<bool> show_img("ui.show_img", true, true);
    ui_set_.push_back(show_img); 
    pangolin::Var<bool> show_coordinate("ui.show_coordinate", true, true);
    ui_set_.push_back(show_coordinate);
    pangolin::Var<bool> save_map("ui.save_map", false, false);
    ui_set_.push_back(save_map);
    pangolin::Var<bool> save_win("ui.save_win", false, false);
    ui_set_.push_back(save_win);

    // 数据显示
    pangolin::CreatePanel("data")
        .SetBounds(pangolin::Attach::Pix(2.0f *PANEL_HEIGHT), pangolin::Attach::Pix(3.0f *PANEL_HEIGHT),
                                    0., pangolin::Attach::Pix(PANEL_WIDTH), (float)WIN_WIDTH_/ (float)WIN_HEIGHT_);
    data_set_.clear();
    pangolin::Var<VecXd> curr_pos("data.pos", VecXd());
    data_set_.push_back(curr_pos);
    pangolin::Var<VecXd> curr_att("data.euler_angle", VecXd());
    data_set_.push_back(curr_att);
    // 原图片显示
    d_img_ = pangolin::CreateDisplay()
        .SetBounds(pangolin::Attach::Pix(1.0f *PANEL_HEIGHT), pangolin::Attach::Pix(2.0f *PANEL_HEIGHT), 
                0., pangolin::Attach::Pix(PANEL_WIDTH), (float)WIN_WIDTH_/ (float)WIN_HEIGHT_)
        .SetLock(pangolin::LockLeft, pangolin::LockBottom);

    imageTexture_ = pangolin::GlTexture(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);
    // 跟踪图片显示
    d_track_ = pangolin::CreateDisplay()
        .SetBounds(0., pangolin::Attach::Pix(1.0f *PANEL_HEIGHT), 
                0., pangolin::Attach::Pix(PANEL_WIDTH), (float)WIN_WIDTH_/ (float)WIN_HEIGHT_)
        .SetLock(pangolin::LockLeft, pangolin::LockBottom);

    trackTexture_ = pangolin::GlTexture(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);
}

void SlamVisualizer::activeAllView(){
    d_cam_.Activate(s_cam_);
}

void SlamVisualizer::drawCubeTest(){
    // Render some stuff
    glColor3f(1.0,0.0,1.0);
    pangolin::glDrawColouredCube();
}

void SlamVisualizer::drawCam(const float scale){

    if(scale < 0){
        cerr << "scale should be positive !\n";
        return;
    }
        
    const float w = 0.2 * scale;
    const float h = w * 0.75;
    const float z = w * 0.8;

    glLineWidth(2 * scale);
    // 绘制相机轮廓线
    glBegin(GL_LINES);
    glColor3f(0.0f,1.0f,1.0f);
	glVertex3f(0,0,0);		glVertex3f(w,h,z);
	glVertex3f(0,0,0);		glVertex3f(w,-h,z);
	glVertex3f(0,0,0);		glVertex3f(-w,-h,z);
	glVertex3f(0,0,0);		glVertex3f(-w,h,z);
	glVertex3f(w,h,z);		glVertex3f(w,-h,z);
	glVertex3f(-w,h,z);		glVertex3f(-w,-h,z);
	glVertex3f(-w,h,z);		glVertex3f(w,h,z);
	glVertex3f(-w,-h,z);    glVertex3f(w,-h,z);
	glEnd();

    return;
}

void SlamVisualizer::drawCamWithPose(Eigen::Vector3d &pos, Eigen::Quaterniond &quat){
    if (!camera_visible_)
        return;

    Eigen::Matrix3d R = quat.toRotationMatrix();

    glPushMatrix();
    std::vector<GLdouble> Twc = {R(0, 0), R(1,0), R(2, 0), 0.,
                                R(0, 1), R(1, 1), R(2, 1), 0.,
                                R(0, 2), R(1, 2), R(2, 2), 0.,
                                pos.x(), pos.y(), pos.z(), 1.};
    glMultMatrixd(Twc.data());
    drawCam();
    glPopMatrix();
}

void SlamVisualizer::drawTraj(vector<Eigen::Vector3d> &traj){
    if(!traj_visible_)
        return;
    glLineWidth(2);
    glBegin(GL_LINES);
    glColor3f(0.f, 1.f, 0.f);
    for(size_t i=0; i<traj.size() - 1; i++){
        glVertex3d(traj[i].x(), traj[i].y(), traj[i].z());
        glVertex3d(traj[i+1].x(), traj[i+1].y(), traj[i+1].z());
    }
    glEnd();
}

void SlamVisualizer::drawCoordinate(){
    if(!coordinate_visible_)
        return;
    // 绘制坐标系
    glLineWidth(3);
    glBegin ( GL_LINES );
	glColor3f ( 1.0f,0.f,0.f );
	glVertex3f( 0,0,0 );
	glVertex3f( 1,0,0 );
	glColor3f( 0.f,1.0f,0.f);
	glVertex3f( 0,0,0 );
	glVertex3f( 0,1,0 );
	glColor3f( 0.f,0.f,1.f);
	glVertex3f( 0,0,0 );
	glVertex3f( 0,0,1 );
	glEnd();
}

void SlamVisualizer::displayImg(cv::Mat& originImg, cv::Mat& trackImg){
    if(!img_visible_)
        return;
    imageTexture_.Upload(originImg.data, GL_BGR, GL_UNSIGNED_BYTE);
        // 显示图像
    d_img_.Activate();
    glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
    imageTexture_.RenderToViewportFlipY(); // 需要反转Y轴，否则输出是倒着的

    trackTexture_.Upload(trackImg.data, GL_BGR, GL_UNSIGNED_BYTE);
        // 显示图像
    d_track_.Activate();
    glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
    trackTexture_.RenderToViewportFlipY(); // 需要反转Y轴，否则输出是倒着的
}

void SlamVisualizer::displayData(Eigen::Vector3d &pos, Eigen::Quaterniond& quat){
    VecXd tmp_pose, tmp_euler;
    tmp_pose.vec_ = pos;
    tmp_euler.vec_ = quat.matrix().eulerAngles(2, 1, 0);// YPR, quat是否需要转置？

    tmp_euler.vec_  *= (180 / M_PI);
    data_set_[0] = tmp_pose;
    data_set_[1] = tmp_euler;
}

void SlamVisualizer::registerUICallback(){
    camera_visible_ = ui_set_[0] ? true : false;
    traj_visible_ = ui_set_[1] ? true : false;
    img_visible_ = ui_set_[2] ? true : false;
    coordinate_visible_ = ui_set_[3] ? true : false;
    
    if(pangolin::Pushed(ui_set_[4]))
        d_cam_.SaveOnRender("map");
    
    if(pangolin::Pushed(ui_set_[5]))
        pangolin::SaveWindowOnRender("win");
}