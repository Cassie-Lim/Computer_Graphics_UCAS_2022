#include "head.h"
int main(int argc, char* argv[]){
    
    clock_t a, b;
    char input_name[256];
    char output_name[256];
    float c;
    int total=0;  // 计算总顶点个数
    if(argc==4){
      strcpy(input_name, argv[1]);
      strcpy(output_name, argv[2]);
      sscanf(argv[3], "%f", &c);
#ifdef DEBUG
      cout<<input_name<<endl;
      cout<<output_name<<endl;
      cout<<c<<endl;
#endif
      // 读取文件失败
      if(OpenMesh::IO::read_mesh(mesh, input_name)==false){
          cout<<"Failed to load in the model, please check your path"<<endl;
          return 0;
      }
      cout<<"Load model successfully"<<endl;
      total = mesh.n_vertices();
      cout<<"[Orig] Model get "<<total<<" vertex in total"<<endl;
      cout<<"[Orig] Model get "<<mesh.n_faces()<<" face in total"<<endl;
      total*=(1-c); // 待删除的顶点个数

      a = time(NULL);
      MeshPropInit();
      MakeElist();
      cout << "Deleting "<<total<<" vertex..."<<endl;
      DeleteVertex(total);
      b = time(NULL);
      cout<<"CPU time = "<<(float)(b-a)<<endl;
      MeshPropDel();
      cout<<"[After] Model get "<<mesh.n_vertices()<<" vertex in total"<<endl;
      cout<<"[After] Model get "<<mesh.n_faces()<<" face in total"<<endl;
      if(OpenMesh::IO::write_mesh(mesh, output_name)){
        cout<<"Saving models with simplification ratio "<<c<<" at "<<output_name<<endl;
        // 显示模型
        cout<<"Loading the model..."<<endl;
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

        glutInitWindowSize(500, 500);
        glutInitWindowPosition(100, 100);
        glutCreateWindow("dragon");

        Initscene();    
        glutDisplayFunc(DrawScene);
        glutSpecialFunc(SpecialKeys);
        glutMouseFunc(mouseClick);

        glutMainLoop();
      }
      else
        cout<<"Fail to save the model"<<endl;
    }
    else
        cout<<"USAGE:./[your_executable_file [input_path] [output_path] [ratio]"<<endl;
}

