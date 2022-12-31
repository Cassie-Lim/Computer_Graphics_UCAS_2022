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

//--------------------------------------for trace.cpp----------------------

#define M_PI 3.1415926
#define MAX_DEPTH 10
#define mix(a, b, ratio) (b)*(ratio)+(a)*(1-(ratio)) 
#define FLOAT_MOD(a, b) (a)-((int)((a)/(b))*(b))
using namespace std;
extern float x_rotate;
extern float y_rotate;

template<typename T>
class Vec_3
{
public:
  // xyz三个分量
  T x, y, z;
  // 三种构造函数
  Vec_3():x(T(0)), y(T(0)), z(T(0)) {}
  Vec_3(T xx): x(xx), y(xx), z(xx) {}
  Vec_3(T xx, T yy, T zz): x(xx), y(yy), z(zz) {}
  // 相关操作符override
  // *
  Vec_3<T> operator * (const T &f) const{
    return Vec_3<T>(x*f, y*f, z*f);
  }
  Vec_3<T> operator * (const Vec_3<T> &v) const{
    return Vec_3<T>(x*v.x, y*v.y, z*v.z);
  }
  // - 
  Vec_3<T> operator - (const Vec_3<T> &v) const{
    return Vec_3<T>(x-v.x, y-v.y, z-v.z);
  }
  Vec_3<T> operator - () const{
    return Vec_3<T>(-x, -y, -z);
  }
  // +
  Vec_3<T> operator + (const Vec_3<T> &v) const{
    return Vec_3<T>(x+v.x, y+v.y, z+v.z);
  }
  // +=
  Vec_3<T> operator += (const Vec_3<T> &v){
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  // +=
  Vec_3<T> operator *= (const Vec_3<T> &v){
    x*= v.x;
    y*= v.y;
    z*= v.z;
    return *this;
  }
  // 向量点乘
  T dot(const Vec_3<T> &v) const{
    return x*v.x + y*v.y + z*v.z;
  }
  // 向量长度
  T length2() const{
    return x*x+y*y+z*z;
  }
  T length() const{
    return sqrt(length2());
  }
  // 向量归一化
  Vec_3& normal(){
    T len = length();
    if(len>0){
      T len_inv = 1/len;
      x *= len_inv;
      y *= len_inv;
      z *= len_inv;
    }
    return *this;
  }
  // 输出向量信息
  friend std::ostream & operator << (std::ostream & os, const Vec_3<T> &v){
    os << "[" << v.x << " "<< v.y << " " << v.z << "]";
    return os;
  }
};
typedef Vec_3<float> Vec_3f;

class ImageTexture
{
public:
    unsigned char* data;
    int nx, ny;

    ImageTexture(){}
    ImageTexture(unsigned char* pixels, int A, int B) : data(pixels), nx(A), ny(B) {}

    //输入u和v，输出对应图片像素的rgb值
    virtual Vec_3f value(float u, float v) const
    {
        int i = int((u)* nx);//求出像素索引
        int j = int((1 - v)*ny - 0.001f);
        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i > nx - 1) i = nx - 1;
        if (j > ny - 1) j = ny - 1;
        float r = int(data[3 * i + 3 * nx*j]) / 255.0f;
        float g = int(data[3 * i + 3 * nx*j + 1]) / 255.0f;
        float b = int(data[3 * i + 3 * nx*j + 2]) / 255.0f;
        return Vec_3f(r, g, b);
    }
};

class Sphere{
public:
  Vec_3f center;  // 球心
  float radius, radius2;  // 球半径&半径平方
  Vec_3f surfaceColor, emissionColor; // 球表面颜色、球自发光颜色（光源）
  float transparency, reflection;   // 球的投射度、反射度
  ImageTexture* tex;      // 指向纹理信息的指针
  // 球的构造函数
  Sphere(
    const Vec_3f &c,
    const float &r,
    const Vec_3f &sc,
    const float &refl = 0,
    const float &transp = 0,
    const Vec_3f &ec = 0,
    ImageTexture* t = NULL) :
    center(c), radius(r), radius2(r*r), surfaceColor(sc), emissionColor(ec),
    transparency(transp), reflection(refl), tex(t)
  {}
  // 使用几何方式，计算光线与球的交点，结果保存至t0,t1
  bool intersect(const Vec_3f &rayorigin, const Vec_3f &raydirection, float &t0, float &t1) const{
    // 光线起点到球中心的向量
    Vec_3f  l = center - rayorigin;
    // 判断光线方向是否与球位置相同
    float tca = l.dot(raydirection);
    if(tca<0)
      return false;
    // 计算光线与球心的最短距离
    float dist = l.dot(l) - tca*tca;
    // 距离大于半径，不相交
    if(dist>radius2)  return false;
    float thc = sqrt(radius2 - dist);
    // t0：第一个相交长度，t1：第二个相交长度
    t0 = tca - thc;
    t1 = tca + thc;
    return true;
  }
  Vec_3f getSurfaceColor(const Vec_3f &rayorigin, const Vec_3f &raydirection, const float &tnear) const{
    if(tex==NULL)
      return surfaceColor;
    // 圆心和交点的连线
    Vec_3f r_vec = (rayorigin - center) + raydirection * tnear;
    r_vec.normal();
    // 获取纹理坐标
    // double phi = atan2(r_vec.z, r_vec.x);
    // double theta = asin(r_vec.y);
    double phi = atan2(r_vec.z, r_vec.x) + FLOAT_MOD(y_rotate, M_PI);
    double theta = asin(r_vec.y) + FLOAT_MOD(x_rotate, M_PI);
    double u = 1-(phi + M_PI) / (2*M_PI);
    double v = (theta + M_PI/2) / M_PI;
    // 返回rgb值
    return tex->value(u, v);
  }
};


// 光线追踪函数，输入光线起点、方向、场景中的球体、光线追踪的最大深度
Vec_3f trace(
  const Vec_3f &rayorigin,
  const Vec_3f &raydirection,
  const std::vector<Sphere> & spheres,
  const int &depth
  );
void render(const std::vector<Sphere> & spheres, float fov);
extern float fov;

//------------------------------- for show_pic.cpp ------------------------------
#define MAX 1000
#define BG_COLOR 1
// #define BG_COLOR 1,0.96,0.93
using namespace std;
// #define WIDTH 960
// #define HEIGHT 720
#define WIDTH 640
#define HEIGHT 480
#define FOV_GAP 5
#define ROTATE_GAP (M_PI/60)
#define LOWER_BOUND 0
#define UPPER_BOUND 100
extern Vec_3f img [WIDTH*HEIGHT];
extern unsigned char img_buf[WIDTH*HEIGHT*3];
void DrawScene();
void mouseClick(int button, int state, int x, int y);
void SpecialKeys(int key, int x, int y);
void dynamic_Sphere();



//------------------------------- for main.cpp ------------------------------
#define SAVE_PIC 0   // 生成单张图片
#define SAVE_ANIM 1   // 保存动画原始帧
#define LIVE_PIC 2   // 实时渲染单张图片（可交互）
#define LIVE_ANIM 3  // 实时渲染动画

// #define MODE LIVE_PIC
#define MODE SAVE_ANIM
#define ADD_TEXTURE
extern std::vector<Sphere> spheres;

#endif