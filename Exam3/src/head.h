#ifndef _HEAD_
#define _HEAD_

#include <GL/glut.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>        
#include <string>
#include <Eigen/Dense>
// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#define MAX 1000
#define SCALE_GAP 0.05
#define LOWER_BOUND SCALE_GAP
#define UPPER_BOUND 1.3 // 自测试的经验值
#define ROTATE_GAP 5.0f
// #define SHOW_EDGE   // 控制是否绘制三角网格的边
// #define DEBUG  // 控制debug信息输出
// #define RE_CALC    // 使用该宏控制是否重新计算参数，已测试0.5，0.75，0.25暂时不行
using namespace Eigen;
using namespace std;


typedef OpenMesh::TriMesh_ArrayKernelT <> MyMesh;
struct item{
  double cost;
  MyMesh::Point optimalv3;
};


struct VECTOR3{
    double X;
    double Y;
    double Z;
};
struct POINT3 {
    double X;
    double Y;
    double Z;
};

struct Texture {
    double TU;
    double TV;
};

struct Normal {
    double NX;
    double NY;
    double NZ;
};

struct Face {
    int V[3];
    int T[3];
    int N[3];
};//由三个v/vt/vn的索引形式组成，一个面含三个顶点

class PIC{
    public:
        vector<POINT3> V;
        vector<Texture> VT;
        vector<Normal> VN;
        vector<Face> F;
};


// show_model.cpp
void CalcNormal();
void Initscene();
void GLCube();
void DrawScene();
void SpecialKeys(int key, int x, int y);
void mouseClick(int button, int state, int x, int y);
// simplify.cpp
static inline Matrix4d ComputeQ(MyMesh::VertexHandle vh);
static inline struct item GetCost(MyMesh::EdgeHandle eh);
void addQproperty();
void addItemproperty();
bool myCmp(MyMesh::EdgeHandle e1, MyMesh::EdgeHandle e2);
void MakeElist();
int updateElist(MyMesh::EdgeHandle eh);
void DeleteVertex(int total);
void MeshPropInit();
void MeshPropDel();

// global var

extern PIC m_pic;
extern MyMesh mesh;
#endif
