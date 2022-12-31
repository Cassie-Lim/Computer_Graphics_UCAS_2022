#include "head.h"
float x_direction = 0.0;
float y_direction = 0.0;
float scale = 0.8;

void CalcNormal(){
    if(!mesh.has_face_normals())
        mesh.request_face_normals();
	// mesh计算出顶点法线
	mesh.update_normals();
}

void Initscene() {
    CalcNormal();
    glClearColor(0.000f, 0.000f, 0.000f, 1.0f);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // 定义翡翠绿材质
    static GLfloat glfMatAmbient[] = { 0.135f,0.2225f,0.1575f,0.95f };
    static GLfloat glfMatDiffuse[] = { 0.54f,0.89f,0.63f,0.95f };
    static GLfloat glfMatSpecular[] = { 0.316228f,0.316228f,0.316228f,0.95f };
    static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
    static GLfloat fShininess = 12.800f;

    //指定 用于光照计算的材质属性，参数face的取值GL_FRONT，GL_BACK，GL_FRONT_AND_BACK，决定材质属性用于哪一面
    glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);      //GL_AMBIENT表示各种光线照射到该材质上，经过很多次反射后最终遗留在环境中的光线强度（颜色）
    glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);      //GL_DIFFUSE表示光线照射到该材质上，经过漫反射后形成的光线强度（颜色）
    glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);    //GL_SPECULAR表示光线照射到该材质上，经过镜面反射后形成的光线强度（颜色）
    glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);    //该材质本身就微微的向外发射光线对应颜色
    glMaterialf(GL_FRONT, GL_SHININESS, fShininess);        //“镜面指数”，取值范围是0到128。该值越小，表示材质越粗糙，点光源发射的光线照射到上面，也可以产生较大的亮点

}

void GLCube(){
#ifdef SHOW_EDGE
    // 绘制边
    glBegin(GL_LINES);
    for (MyMesh::HalfedgeIter he_h = mesh.halfedges_begin(); he_h != mesh.halfedges_end(); ++he_h) {
        glVertex3fv(mesh.point(mesh.from_vertex_handle(*he_h)).data());
        glVertex3fv(mesh.point(mesh.to_vertex_handle(*he_h)).data());
    }
    glEnd();
#endif
    // 绘制面
    for (MyMesh::FaceIter fh = mesh.faces_begin(); fh != mesh.faces_end(); ++fh) {
		glBegin(GL_TRIANGLES);
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*fh); fv_it.is_valid(); ++fv_it) {
			glNormal3fv(mesh.normal(*fv_it).data());
			glVertex3fv(mesh.point(*fv_it).data());
		}
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
    glRotatef(x_direction, 1.0f, 0.0f, 0.0f);
    // 左右旋转
    glRotatef(y_direction, 0.0f, 1.0f, 0.0f); 
    // 放大缩小
    glScalef(scale, scale, scale);


    GLCube();

    glFlush();
    glutSwapBuffers();
}

// 键盘调整视角，每次操作步幅ROTATE_GAP
void SpecialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) 
        y_direction -= ROTATE_GAP;
    else if (key == GLUT_KEY_RIGHT) 
        y_direction += ROTATE_GAP;
    else if (key == GLUT_KEY_UP) 
        x_direction -= ROTATE_GAP;
    else if (key == GLUT_KEY_DOWN) 
        x_direction += ROTATE_GAP;
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
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(scale<=LOWER_BOUND)    // 避免scale减小为非正值
            printf("Reach the scale lower bound!\n");
        else
            scale -= SCALE_GAP;
    }
    DrawScene();
}
