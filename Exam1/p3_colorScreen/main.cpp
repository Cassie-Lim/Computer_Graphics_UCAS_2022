#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#define PI acos(-1)
#define BLACK 0,0,0
#define WHITE 1,1,1
#define GREY    190.0/255, 190.0/255, 190.0/255
#define DIMGREY 105.0/255, 105.0/255, 105.0/255
#define GOLDEN  255.0/255, 215.0/255, 89.0/255
#define RED     205.0/255, 79.0/255,  57.0/255
#define BLUE    145.0/255, 206.0/255, 222.0/255
//-----------------------------------------API参考了课程ppt----------------------------------------------------
void glTri(double x1, double y1, double x2, double y2, double x3, double y3, int mode);
void glRect(double leftX, double leftY, double rightX, double rightY, int mode);
void glArc(double x, double y, double start_angle, double end_angle, double radius, int mode);
//-----------------------------------------------------------------------------------------------------------
void glArcPoint(double x, double y, double start_angle, double end_angle, double radius);    // 只绘制散点
void glRectSmooth(double leftX, double leftY, double rightX, double rightY, double radius, int mode); // 画四角为圆弧的长方体
void init(void)
{
  glClearColor(WHITE, 0.0f);
  return;
}
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glLineWidth(2.0f);
  // 屏幕外框
  glColor3f(GREY);
  glRect(-0.4, -0.4, 0.4, 0.4, GL_POLYGON); 
  // 屏幕内框
  glColor3f(BLUE);
  glRectSmooth(-0.35, -0.35, 0.35, 0.35, 0.05, GL_POLYGON);
  // 基座
  glColor3f(DIMGREY);
  glRectSmooth(-0.28, -0.62, 0.28, -0.47, 0.03, GL_POLYGON);
  glColor3f(BLACK);
  glRectSmooth(-0.28, -0.62, 0.28, -0.47, 0.03, GL_LINE_LOOP);  // 基座的描线需特殊处理
  glColor3f(DIMGREY);
  glRect(-0.20, -0.55, 0.20, -0.40, GL_POLYGON);
  // 屏幕内图像
  glColor3f(GOLDEN);
  glTri(-0.15, -0.15, 0.15, -0.15, 0, 0.2, GL_POLYGON);
  glColor3f(RED);
  glArc(0, 0, 0, 2*PI, 0.05, GL_POLYGON);
  // 统一描线
  glColor3f(BLACK);
  {
    glRect(-0.4, -0.4, 0.4, 0.4, GL_LINE_LOOP); 
    // 屏幕内框
    glRectSmooth(-0.35, -0.35, 0.35, 0.35, 0.05, GL_LINE_LOOP);
    // 基座
    glRect(-0.20, -0.55, 0.20, -0.40, GL_LINE_LOOP);
    // 屏幕内图像
    glTri(-0.15, -0.15, 0.15, -0.15, 0, 0.2, GL_LINE_LOOP);
    glArc(0, 0, 0, 2*PI, 0.05, GL_LINE_LOOP);
  }
  glFlush();
  return;
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(600, 400);
  glutCreateWindow("OpenGL 3D View");
  glutDisplayFunc(display);
  init();
  glutMainLoop();
  return 0;
}

//画三角形,传入三个点的坐标, mode：GL_LINE_LOOP / GL_POLYGON
void glTri(double x1, double y1, double x2, double y2, double x3, double y3, int mode) {
  //画封闭线
  glBegin(mode);
  //一点
  glVertex2f(x1, y1);
  //二点
  glVertex2f(x2, y2);
  //三点
  glVertex2f(x3, y3);
  //结束画线
  glEnd();
}
//画矩形,传入的是左下角XY坐标和右上角XY坐标
void glRect(double leftX, double leftY, double rightX, double rightY, int mode) {
  //画封闭曲线
  glBegin(mode);
  //左下角
  glVertex2f(leftX, leftY);
  //右下角
  glVertex2f(rightX, leftY);
  //右上角
  glVertex2f(rightX, rightY);
  //左上角
  glVertex2f(leftX, rightY);
  //结束画线
  glEnd(); 
}

//画弧线,相对偏移量XY,开始的弧度,结束的弧度,半径
void glArc(double x, double y, double start_angle, double end_angle, double radius, int mode){
  //开始绘制曲线
  glBegin(mode);
  glArcPoint(x, y, start_angle, end_angle, radius);
  //结束绘画
  glEnd();
}
void glArcPoint(double x, double y, double start_angle, double end_angle, double radius){
  //每次画增加的弧度
  double delta_angle = PI / 180;
  //画圆弧
  for (double i = start_angle; i <= end_angle; i += delta_angle){
    //绝对定位加三角函数值
    double vx = x + radius * cos(i);
    double vy = y + radius * sin(i);
    glVertex2f(vx, vy);
  }
}
void glRectSmooth(double leftX, double leftY, double rightX, double rightY, double radius, int mode){
  glBegin(mode);
  // 为了画圆弧，需要缩进(shrink)一部分
  double s_leftX, s_leftY, s_rightX, s_rightY;  
  s_leftX = leftX + radius;
  s_leftY = leftY + radius;
  s_rightX = rightX - radius;
  s_rightY = rightY - radius;

  // Arc画屏幕内框的圆弧的散点
  glArcPoint( s_leftX,  s_leftY,       PI, 1.5 * PI, radius);
  glArcPoint(s_rightX,  s_leftY, 1.5 * PI,   2 * PI, radius);
  glArcPoint(s_rightX, s_rightY,        0, 0.5 * PI, radius);
  glArcPoint( s_leftX, s_rightY, 0.5 * PI,       PI, radius);

  glEnd();
}