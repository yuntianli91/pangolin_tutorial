#include <pangolin/pangolin.h>
#include <stdio.h>
using namespace std;

int main(int argc, char** argv){
    FILE* fp_gt;
    // 请自行修改数据集路径
    
    fp_gt = fopen("/home/yuntian/dataset/MH_05_difficult/mav0/state_groundtruth_estimate0/data.csv", "r");
    if(fp_gt == nullptr){
        cout << "failed to open file !\n";
        return -1;
    }
    // 跳过第一行
    char fl_buf[1024];
    fgets(fl_buf, sizeof(fl_buf), fp_gt);
    // 创建数据寄存器    
    ulong time_stamp(0);
    double px(0.), py(0.), pz(0.);
    double qw(0.), qx(0.), qy(0.), qz(0.);
    double vx(0.), vy(0.), vz(0.);
    double bwx(0.), bwy(0.), bwz(0.), bax(0.), bay(0.), baz(0.);
    vector<Eigen::Vector3d> traj;
    // 初始化视窗
    pangolin::CreateWindowAndBind("camera_pose", 752 * 2, 480 * 2);
    glEnable(GL_DEPTH_TEST);

    pangolin::OpenGlRenderState s_cam_ = pangolin::OpenGlRenderState(
        pangolin::ProjectionMatrix(752 * 2, 480 * 2, 420, 420, 320, 240, 0.1, 1000),
        pangolin::ModelViewLookAt(5, -3, 5, 0, 0, 0, pangolin::AxisZ)
    );
        
    pangolin::View& d_cam_ = pangolin::CreateDisplay()
        .SetBounds(0., 1., 0., 1., -752/ 480.)
        .SetHandler(new pangolin::Handler3D(s_cam_));
    
    while (!feof(fp_gt))
    {
        // ========================== 一系列常规操作 ================================== //
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        d_cam_.Activate(s_cam_);
        // ======================== 从groung_truth中读取位姿 ========================== //
        fscanf(fp_gt, "%lu,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
            &time_stamp, &px, &py, &pz,
            &qw, &qx, &qy, &qz,
            &vx, &vy, &vz,
            &bwx, &bwy, &bwz,
            &bax, &bay, &baz);

        Eigen::Quaterniond quat(qw, qx, qy, qz); //quat是否要转置？
        Eigen::Vector3d pos(px, py, pz);
        traj.push_back(pos);

          // -------- 绘制坐标系 -------- //
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
        // -------- 绘制随位姿变化的相机模型 -------- //
        // 构建位姿变换矩阵，pangolin中为列主序
        Eigen::Matrix3d R = quat.toRotationMatrix();

        glPushMatrix();
        std::vector<GLdouble> Twc = {R(0, 0), R(1,0), R(2, 0), 0.,
                                R(0, 1), R(1, 1), R(2, 1), 0.,
                                R(0, 2), R(1, 2), R(2, 2), 0.,
                                pos.x(), pos.y(), pos.z(), 1.};
        glMultMatrixd(Twc.data());
       // 绘制相机轮廓线
        const float w = 0.2;
        const float h = w * 0.75;
        const float z = w * 0.6;

        glLineWidth(2); 
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
        glPopMatrix();
        // -------- 绘制相机轨迹 --------//
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3f(0.f, 1.f, 0.f);
        for(size_t i=0; i<traj.size() - 1; i++){
            glVertex3d(traj[i].x(), traj[i].y(), traj[i].z());
            glVertex3d(traj[i+1].x(), traj[i+1].y(), traj[i+1].z());
        }
        glEnd();
    
        pangolin::FinishFrame();
        
        if(pangolin::ShouldQuit())
            break;
    }

    return 0;
}

