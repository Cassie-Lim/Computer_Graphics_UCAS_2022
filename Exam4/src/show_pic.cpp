#include "head.h"
float x_rotate = 0;
float y_rotate = 0;
void dynamic_Sphere(){
    for(auto i=0; i<spheres.size(); i++){
        Vec_3f tmp = 0;
        float delta_y_rotate =  ROTATE_GAP;
        tmp.x = (spheres[i].center.x - spheres[1].center.x) * cos(delta_y_rotate) + (spheres[i].center.z - spheres[1].center.z) * sin(delta_y_rotate);
        tmp.y = spheres[i].center.y - spheres[1].center.y;
        tmp.z = -(spheres[i].center.x - spheres[1].center.x) * sin(delta_y_rotate) + (spheres[i].center.z - spheres[1].center.z) * cos(delta_y_rotate);
        spheres[i].center.x = tmp.x + spheres[1].center.x;
        spheres[i].center.y = tmp.y + spheres[1].center.y;
        spheres[i].center.z = tmp.z + spheres[1].center.z;
    }
    for(auto i=0; i<spheres.size(); i++){
        Vec_3f tmp = 0;
        float delta_x_rotate = ROTATE_GAP;
        tmp.x = spheres[i].center.x - spheres[1].center.x;
        tmp.y = (spheres[i].center.y - spheres[1].center.y) * cos(delta_x_rotate) - (spheres[i].center.z - spheres[1].center.z) * sin(delta_x_rotate);
        tmp.z = (spheres[i].center.y - spheres[1].center.y) * sin(delta_x_rotate) + (spheres[i].center.z - spheres[1].center.z) * cos(delta_x_rotate);
        spheres[i].center.x = tmp.x + spheres[1].center.x;
        spheres[i].center.y = tmp.y + spheres[1].center.y;
        spheres[i].center.z = tmp.z + spheres[1].center.z;
    }
}
void DrawScene()
{
#if MODE==LIVE_ANIM
    while(1){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        dynamic_Sphere();
        render(spheres, fov); 
        glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, img_buf);

        glFlush();
        glutSwapBuffers();
    }
#else
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    render(spheres, fov); 
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, img_buf);

    glFlush();
    glutSwapBuffers();
#endif    
}


// 鼠标左击缩小fov，右击放大fov，每次操作步幅为FOV_GAP
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if(fov>=UPPER_BOUND)
            printf("Reach the fov upper bound!\n");
        else
            fov+=FOV_GAP;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(fov<=LOWER_BOUND)    // 避免fov减小为非正值
            printf("Reach the fov lower bound!\n");
        else
            fov-=FOV_GAP;
    }
    DrawScene();
}

// 键盘调整视角，每次操作步幅ROTATE_GAP
void SpecialKeys(int key, int x, int y) {
    // 旋转球体
    // 绕y轴旋转
    if(key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT){
        float delta_y_rotate = GLUT_KEY_LEFT? - ROTATE_GAP : ROTATE_GAP;
        y_rotate += delta_y_rotate;
        for(auto i=0; i<spheres.size(); i++){
            Vec_3f tmp = 0;
            tmp.x = (spheres[i].center.x - spheres[1].center.x) * cos(delta_y_rotate) + (spheres[i].center.z - spheres[1].center.z) * sin(delta_y_rotate);
            tmp.y = spheres[i].center.y - spheres[1].center.y;
            tmp.z = -(spheres[i].center.x - spheres[1].center.x) * sin(delta_y_rotate) + (spheres[i].center.z - spheres[1].center.z) * cos(delta_y_rotate);
            spheres[i].center.x = tmp.x + spheres[1].center.x;
            spheres[i].center.y = tmp.y + spheres[1].center.y;
            spheres[i].center.z = tmp.z + spheres[1].center.z;
        }
    }
    // 绕x轴旋转
    else if(key == GLUT_KEY_UP || key == GLUT_KEY_DOWN){
        float delta_x_rotate = GLUT_KEY_UP? - ROTATE_GAP : ROTATE_GAP;
        x_rotate += delta_x_rotate;
        for(auto i=0; i<spheres.size(); i++){
            Vec_3f tmp = 0;
            tmp.x = spheres[i].center.x - spheres[1].center.x;
            tmp.y = (spheres[i].center.y - spheres[1].center.y) * cos(delta_x_rotate) - (spheres[i].center.z - spheres[1].center.z) * sin(delta_x_rotate);
            tmp.z = (spheres[i].center.y - spheres[1].center.y) * sin(delta_x_rotate) + (spheres[i].center.z - spheres[1].center.z) * cos(delta_x_rotate);
            spheres[i].center.x = tmp.x + spheres[1].center.x;
            spheres[i].center.y = tmp.y + spheres[1].center.y;
            spheres[i].center.z = tmp.z + spheres[1].center.z;
        }
    }
    DrawScene();
}