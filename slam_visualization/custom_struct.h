#pragma once
#include <iostream>
#include <iomanip>
#include <Eigen/Dense>
using namespace std;

// 创建一个VectorXd用于pangolin::Var输出数据
// 根据pangolin文档，自定义类型需要重载输入输出流操作符
struct VecXd
{
    Eigen::VectorXd vec_ = Eigen::Vector3d::Zero();
};
// 使用inline休息避免头文件中的非模板、非成员重复包含
inline ostream& operator << (ostream& out, const VecXd& r){
    int N = r.vec_.size();
    out.setf(ios::fixed);
    out << '=' << " [";
    for(int i=0; i< N-1; i++){
        out << setprecision(2) << r.vec_(i) << ", ";
    }
    out << r.vec_(N-1) << "]";
    return out;
}

inline istream& operator >> (istream& in, VecXd& r){
    return in;
}
