

#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>        
#include <string>
#include <vector>

using namespace std;
#define MAX 1000
#define SCALE_GAP 0.05
#define LOWER_BOUND SCALE_GAP
#define UPPER_BOUND 1.3 // 自测试的经验值
#define ROTATE_GAP 5.0f
#define CALC_NORMAL // 自计算法向的开关宏
float x_rotate = 0.0;
float y_rotate = 0.0;
float scale = 0.8;

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

PIC m_pic;

void ReadPic() {
    ifstream ifs("./bunny.obj");
    string s;
    Face* f;
    POINT3* v;
    Normal* vn;
    Texture* vt;
    while (getline(ifs, s)) {
        if (s.length() < 2) continue;
        if (s[0] == 'v') {
            if (s[1] == 't') {
                istringstream in(s);
                vt = new Texture();
                string head;
                in >> head >> vt->TU >> vt->TV;
                m_pic.VT.push_back(*vt);
            }
            else if (s[1] == 'n') {
                istringstream in(s);
                vn = new Normal();
            #ifndef CALC_NORMAL
                string head;
                in >> head >> vn->NX >> vn->NY >> vn->NZ;
            #else
                vn->NX=vn->NY=vn->NZ=0;
            #endif
                m_pic.VN.push_back(*vn);
            }
            else {
                istringstream in(s);
                v = new POINT3();
                string head;
                in >> head >> v->X >> v->Y >> v->Z;
                m_pic.V.push_back(*v);
            }
        }
        else if (s[0] == 'f') {
            for (int k = s.size() - 1; k >= 0; k--) {
                if (s[k] == '/') s[k] = ' ';
            }
            istringstream in(s);
            f = new Face();
            string head;
            in >> head;
            int i = 0;
            while (i < 3) {
                //确定顶点对应的vertex index（从0开始计数）
                if (m_pic.V.size() != 0) {
                    in >> f->V[i];
                    f->V[i] -= 1;
                }
                //确定顶点对应的纹理 index（模型bunny未给出）
                if (m_pic.VT.size() != 0) {
                    in >> f->T[i];
                    f->T[i] -= 1;
                }
                //确定顶点对应的法向量 index（从0开始计数）
                if (m_pic.VN.size() != 0) {
                    in >> f->N[i];
                    f->N[i] -= 1;
                }
                i++;
            }
            m_pic.F.push_back(*f);
        }
    }
}
void CalcNormal(){
    Face *f;
    POINT3 v0, v1, v2;      // 平面的三个顶点
    Normal *vn;
    double norm;
    cout<<"Calculating normals..."<<endl;
    for(long long unsigned int i=0; i<m_pic.F.size(); i++){
        f= &m_pic.F[i];
        // step1 根据三个顶点计算平面法向量
        v0 = m_pic.V[f->V[0]];
        v1 = m_pic.V[f->V[1]];
        v2 = m_pic.V[f->V[2]];
        VECTOR3 v0_v1 = {v1.X - v0.X, v1.Y - v0.Y, v1.Z - v0.Z};
        VECTOR3 v0_v2 = {v2.X - v0.X, v2.Y - v0.Y, v2.Z - v0.Z};
        VECTOR3 fv =   {v0_v1.Y*v0_v2.Z - v0_v1.Z*v0_v2.Y,
                       -v0_v1.X*v0_v2.Z + v0_v1.Z*v0_v2.X,
                        v0_v1.X*v0_v2.Y - v0_v1.Y*v0_v2.X}; // 平面法向量（未归一化，含面积信息）
        // step2 将带权法向量加到每一个顶点的法向向量上
        m_pic.VN[f->N[0]].NX += fv.X;
        m_pic.VN[f->N[1]].NX += fv.X;
        m_pic.VN[f->N[2]].NX += fv.X;

        m_pic.VN[f->N[0]].NY += fv.Y;
        m_pic.VN[f->N[1]].NY += fv.Y;
        m_pic.VN[f->N[2]].NY += fv.Y;

        m_pic.VN[f->N[0]].NZ += fv.Z;
        m_pic.VN[f->N[1]].NZ += fv.Z;
        m_pic.VN[f->N[2]].NZ += fv.Z;    
    }
    // step3 遍历顶点，归一化顶点方向向量
    for(long long unsigned int i=0; i<m_pic.VN.size(); i++){
        vn = &m_pic.VN[i];
        norm = sqrt(vn->NX*vn->NX + vn->NY*vn->NY + vn->NZ*vn->NZ);
        vn->NX = vn->NX/norm;
        vn->NY = vn->NY/norm;
        vn->NZ = vn->NZ/norm;
    }
}

void Initscene() {
    ReadPic();
#ifdef CALC_NORMAL
    CalcNormal();
#endif
    glClearColor(0.000f, 0.000f, 0.000f, 1.0f);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // 定义黄铜材质
    static GLfloat glfMatAmbient[] = { 0.33f, 0.22f, 0.03f, 1.0f };
    static GLfloat glfMatDiffuse[] = { 0.78f, 0.57f, 0.11f, 1.0f };
    static GLfloat glfMatspecular[] = { 0.99f, 0.91f, 0.81f,1.0f };
    static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
    static GLfloat fShininess = 27.800f;

    //指定 用于光照计算的材质属性，参数face的取值GL_FRONT，GL_BACK，GL_FRONT_AND_BACK，决定材质属性用于哪一面
    glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);      //GL_AMBIENT表示各种光线照射到该材质上，经过很多次反射后最终遗留在环境中的光线强度（颜色）
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);      //GL_DIFFUSE表示光线照射到该材质上，经过漫反射后形成的光线强度（颜色）
    glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatspecular);    //GL_SPECULAR表示光线照射到该材质上，经过镜面反射后形成的光线强度（颜色）
    glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);    //该材质本身就微微的向外发射光线对应颜色
    glMaterialf(GL_FRONT, GL_SHININESS, fShininess);        //“镜面指数”，取值范围是0到128。该值越小，表示材质越粗糙，点光源发射的光线照射到上面，也可以产生较大的亮点

}

void GLCube(){
    for (int i = 0; i < m_pic.F.size(); i++){
        // drawing begin
        glBegin(GL_TRIANGLES);
        for(int j=0; j <3; j++){
            //如有纹理信息，完成纹理映射
            if (m_pic.VT.size() != 0)
                glTexCoord2f(m_pic.VT[m_pic.F[i].T[j]].TU, m_pic.VT[m_pic.F[i].T[j]].TV);  	
            glNormal3f(m_pic.VN[m_pic.F[i].N[j]].NX, m_pic.VN[m_pic.F[i].N[j]].NY, m_pic.VN[m_pic.F[i].N[j]].NZ);
            glVertex3f(m_pic.V[m_pic.F[i].V[j]].X / MAX, m_pic.V[m_pic.F[i].V[j]].Y / MAX, m_pic.V[m_pic.F[i].V[j]].Z / MAX);
        }
        // drawing end
        glEnd();
    }
}

void DrawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glLoadIdentity(); 

    // Add a light source
    GLfloat glfLight[] = { -4.0f, 4.0f, -4.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, glfLight);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    // 上下旋转
    glRotatef(x_rotate, 1.0f, 0.0f, 0.0f);
    // 左右旋转
    glRotatef(y_rotate, 0.0f, 1.0f, 0.0f); 
    // 放大缩小
    glScalef(scale, scale, scale);


    GLCube();

    glFlush();
    glutSwapBuffers();
}

// 键盘上下左右键调整视角
void SpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP) 
        x_rotate -= ROTATE_GAP;
    else if (key == GLUT_KEY_DOWN) 
        x_rotate += ROTATE_GAP;
    else if (key == GLUT_KEY_LEFT) 
        y_rotate -= ROTATE_GAP;
    else if (key == GLUT_KEY_RIGHT) 
        y_rotate += ROTATE_GAP;
    DrawScene();
}

// 鼠标左击缩小，右击放大，每次操作步幅为SCALE_GAP
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if(scale>=UPPER_BOUND)
            printf("Reach the scale upper bound!\n");
        else
            scale += SCALE_GAP;
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(scale<=LOWER_BOUND)    // 避免scale减小为非正值
            printf("Reach the scale lower bound!\n");
        else
            scale -= SCALE_GAP;
    }
    DrawScene();
}


int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("bunny");

    Initscene();    
    glutDisplayFunc(DrawScene);
    glutSpecialFunc(SpecialKeys);
    glutMouseFunc(mouseClick);

    glutMainLoop();
    return 0;
}



/*
顶点数据(Vertex data)：
    v 几何体顶点(Geometric vertices)
    vt 贴图坐标点(Texture vertices)
    vn 顶点法线(Vertex normals)
    vp 参数空格顶点 (Parameter space vertices)

自由形态曲线(Free-form curve)/表面属性(surface attributes):
    deg 度(Degree)
    bmat 基础矩阵(Basis matrix)
    step 步尺寸(Step size)
    cstype 曲线或表面类型 (Curve or surface type)

元素(Elements):
    p 点(Point)
    l 线(Line)
    f 面(Face)
    curv 曲线(Curve)
    curv2 2D曲线(2D curve)
    surf 表面(Surface) 
 */