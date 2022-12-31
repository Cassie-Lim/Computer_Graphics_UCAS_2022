#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#define N 100
int  r1, r2, value, color, shape, mod, num=0;
int a[N*6];
void color_menu(int value){
    color = value;  // 存储颜色序号
}
void shape_menu(int value){
    shape = value;  // 存储形状序号
}
void mymenu(int value){
    if(value==2){
        num=0;  // 把原有图形清空
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_STENCIL_BUFFER_BIT);
        glFlush();
        printf("清屏\n");
    }
    if(value==3)
        mod=0;  // 进入绘制模式
}
void menu(){
    //创建颜色子菜单
    r1 = glutCreateMenu(color_menu);
    glutAddMenuEntry("Green", 1);
    glutAddMenuEntry("Red", 2);
    glutAddMenuEntry("Blue", 3);
    //创建图形子菜单
    r2 = glutCreateMenu(shape_menu);
    glutAddMenuEntry("Line segment", 1);
    glutAddMenuEntry("Rect", 2);
    glutAddMenuEntry("Triangle", 3);
    //创建菜单，连接子菜单
    glutCreateMenu(mymenu);
    glutAddSubMenu("Choose color", r1);
    glutAddSubMenu("Choose shape", r2);
    glutAddMenuEntry("Clear Screen", 2);
    glutAddMenuEntry("Draw Mode", 3);
    //鼠标右键控制并选择菜单按钮
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void drawOBjects(int mode){
    int i;
    for(i=0; i<num; i++){
        switch (int(a[i*6+4]))  // 存储的颜色
        {
        case 1:
            glColor3f(0.0, 1.0, 0.0);
            break;
        case 2:
            glColor3f(1.0, 0.0, 0.0);
            break;
        case 3:
            glColor3f(0.0, 0.0, 1.0);
            break;
        }
        switch (int(a[i*6+5]))  // 存储的形状
        {
        case 1:
            glBegin(GL_LINES);
            glVertex2f(a[i*6], a[i*6+1]);
            glVertex2f(a[i*6+2], a[i*6+3]);
            glEnd();
            break;
        case 2:
            glRectf(a[i*6], a[i*6+1], a[i*6+2], a[i*6+3]);
            break;
        case 3:
            glBegin(GL_TRIANGLES);
            glVertex2f(a[i*6], a[i*6+1]);
            glVertex2f(a[i*6+2], a[i*6+3]);
            glVertex2f(a[i*6], a[i*6+3]);
            glEnd();
            break;
        default:
            break;
        }
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    drawOBjects(GL_RENDER);
    glFlush();
}
void mouseClick(int button, int state, int x, int y){
    // 存储起点坐标
    if ((mod== 0) && (num <= N) && (state== GLUT_DOWN) && (button== GLUT_LEFT_BUTTON)){
        a[num * 6] = (x); 
        a[num * 6 + 1] = (400 -y);
    }
    // 存储终点坐标，绘制颜色，图形种类
    if((mod==0) && (num<=N) && (state==GLUT_UP) && (button==GLUT_LEFT_BUTTON) && (x!=a[num*4]||y!=a[num*4+1])){
        a[num*6+2] = x;
        a[num*6+3] = 400-y;
        a[num*6+4] = color;
        a[num*6+5] = shape;
        printf("num:%d\n", num+1);
        num++;
        display();
    }
}
void init(){
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);

    glutCreateWindow("Simple");
    menu(); // 建立菜单

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //设置相机位置
    gluLookAt(200, 200, 100, 200, 200, 0, 0, 1, 0);
    //设置为正交投影
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-200, 200, -200, 200, 0, 200);
}
int main(int argc, char *argv[]){
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);
    glutMainLoop();
    return 0;
}

