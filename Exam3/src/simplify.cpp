#include "head.h"
MyMesh mesh;
vector<MyMesh::EdgeHandle> elist;  // 边容器
OpenMesh::VPropHandleT<Matrix4d>q; 
OpenMesh::EPropHandleT<struct item>cost_v; 
static inline Matrix4d ComputeQ(MyMesh::VertexHandle vh){
  Matrix4d tmpQ;
  tmpQ.fill(0);
  auto vertex = mesh.point(vh);  // 从句柄获得点
  // 遍历和目标顶点相邻的面，计算损失矩阵坐标
  for(auto v_fh = mesh.vf_begin(vh); v_fh!=mesh.vf_end(vh); v_fh++){  // v_fh：vertex相邻的face的handle
    if(!mesh.is_valid_handle(*v_fh))
      continue;
    Matrix4d Kp;
    auto normal = mesh.normal(*v_fh);
    // double tmp = -vertex[0]*normal[0]-vertex[1]*normal[1]-vertex[2]*normal[2];
    double tmp = -(double)(vertex|normal);
    MatrixXd plane(1,4);
    plane << normal[0], normal[1], normal[2], tmp;
  #ifdef DEBUG
    cout<<"Normal for face "<<*v_fh<<" "<<normal[0]<<","<<normal[1]<<","<<normal[2]<<endl;
    cout<<"Vertex|normal:"<<tmp<<endl;
  #endif
    Kp = plane.transpose() * plane;
    tmpQ += Kp; 
  }
  return tmpQ;
}

static inline struct item GetCost(MyMesh::EdgeHandle eh){
  auto he_h = mesh.halfedge_handle(eh, 0);
  auto pti = mesh.from_vertex_handle(he_h);
  auto ptj = mesh.to_vertex_handle(he_h);
  auto pi_Q = mesh.property(q, pti);
  auto pj_Q = mesh.property(q, ptj);
  Matrix4d m = pi_Q + pj_Q;
  Matrix4d Qsolve = m;
  Qsolve(3, 0)=Qsolve(3, 1)=Qsolve(3, 2)=0.0;
  Qsolve(3, 3)=1.0;
  Vector4d tmp(0.0, 0.0, 0.0, 1.0);
  Vector4d optimalv4 = Qsolve.inverse() * tmp; // 矩阵求逆，若不可逆，算出的是伪逆
  struct item best;
  best.cost = optimalv4.transpose() * m * optimalv4;
  best.optimalv3[0] = optimalv4[0];
  best.optimalv3[1] = optimalv4[1];
  best.optimalv3[2] = optimalv4[2];
  return best;
}
// 为每个顶点增加损失矩阵属性
void addQproperty(){
    cout<<"Adding q property..."<<endl;
    if(!mesh.get_property_handle(q, "Item Property"))
      mesh.add_property(q, "Q Property");
    // // 获取法向信息
    if(!mesh.has_face_normals())
      mesh.request_face_normals();
    if(!mesh.has_vertex_normals())
      mesh.request_vertex_normals();
    mesh.update_normals();
    // 遍历所有顶点
    for(auto vh=mesh.vertices_begin(); vh!=mesh.vertices_end(); ++vh){  // vh:vertex handle
      mesh.property(q, *vh) = ComputeQ(*vh);
    }
    cout<<"Adding q property finished"<<endl;
}
// 为每条边增加item类型的信息
void addItemproperty(){
    cout<<"Adding item property..."<<endl;
    if(!mesh.get_property_handle(cost_v, "Item Property"))
      mesh.add_property(cost_v, "Item Property");
    // 遍历所有边
    for(auto eh=mesh.edges_begin(); eh!=mesh.edges_end(); ++eh){
      struct item tmp = GetCost(*eh);
      mesh.property(cost_v, *eh)=tmp;
    }
    cout<<"Adding item property finished"<<endl;
}


bool myCmp(MyMesh::EdgeHandle e1, MyMesh::EdgeHandle e2){
  if(!mesh.is_valid_handle(e1) || !mesh.is_valid_handle(e2))
    return 1;
  auto it1 = mesh.property(cost_v, e1);
  auto it2 = mesh.property(cost_v, e2);
  return it1.cost < it2.cost;
}

void MakeElist(){
  elist.clear();
  for(auto eh=mesh.edges_begin(); eh!=mesh.edges_end(); ++eh){
    elist.push_back(*eh);
  }
  // 刚开始使用快排，优于插入
  sort(elist.begin(), elist.end(), myCmp);
  cout<<"Sort "<<elist.size()<<" edges in total"<<endl;
}

#ifdef RE_CALC
  // 更新时重新计算相关参数
  int updateElist(MyMesh::EdgeHandle eh){
    if(!mesh.is_valid_handle(eh)){
      return 0;
    }
    auto he_h = mesh.halfedge_handle(eh, 0);
    auto from = mesh.from_vertex_handle(he_h);
    auto to = mesh.to_vertex_handle(he_h);
    // 如果是边界，不进行收缩
    if(mesh.is_boundary(eh) || mesh.is_boundary(to))
      return 0;
    // 检查是否可以做collapse操作，避免collapse时报错
    if(!mesh.is_collapse_ok(he_h))
      return 0;
    // 遍历坍缩目的点所相邻的边
    for(auto v_eh = mesh.ve_begin(to); v_eh!=mesh.ve_end(to); v_eh++){
      // 从list中删除
      auto del_e = *v_eh;
      auto it = find(elist.begin(), elist.end(), del_e);
      if(it!=elist.end()){
        elist.erase(it);
  #ifdef DEBUG
        cout<<"Deleting edge "<<del_e<<" with cost"<<mesh.property(cost_v, del_e).cost<<endl;
      }
      else{
        cout<<"Fail to find the edge connected with 'to'"<<endl;
  #endif
      }
    }
    // 遍历待移动点所相邻的边 
    for(auto v_eh = mesh.ve_begin(from); v_eh!=mesh.ve_end(from); v_eh++){
      auto del_e = *v_eh;
      auto it = find(elist.begin(), elist.end(), del_e);
      if(it!=elist.end())
        elist.erase(it);
  #ifdef DEBUG
      else
        cout<<"Fail to find the edge connected with 'from'"<<endl;
  #endif
    }
    auto vbest = mesh.property(cost_v, eh); 
    mesh.set_point(to, vbest.optimalv3);
    mesh.collapse(he_h);
    mesh.garbage_collection();
    // 插入更新后的边
    for(auto v_eh = mesh.ve_begin(to); v_eh!=mesh.ve_end(to) ; v_eh++){
      if(!mesh.is_valid_handle(*v_eh))
        continue;
      auto he_h = mesh.halfedge_handle(*v_eh, 0);
      auto pti = mesh.from_vertex_handle(he_h);
      auto ptj = mesh.to_vertex_handle(he_h);
      // 重新计算Q property
      mesh.property(q, pti) = ComputeQ(pti);
      mesh.property(q, ptj) = ComputeQ(ptj);
      // 重新计算item property
      mesh.property(cost_v, *v_eh) = GetCost(*v_eh);
      auto pos = upper_bound(elist.begin(), elist.end(), *v_eh, myCmp);
      elist.insert(pos, *v_eh);
  #ifdef DEBUG
      cout<<"List size "<<elist.size()<<endl;
      cout<<"Adding edge "<<*v_eh<<" with cost"<<mesh.property(cost_v, *v_eh).cost<<endl;
      cout<<"Pos "<<*pos<<endl;
      cout<<"Adding finished."<<endl;
  #endif
    }
    return 1;
  }
#else
  // 更新时只删除无效边
  int updateElist(MyMesh::EdgeHandle eh){
    if(!mesh.is_valid_handle(eh)){
      return 0;
    }
    auto he_h = mesh.halfedge_handle(eh, 0);
    auto to = mesh.to_vertex_handle(he_h);
    // 如果是边界，不进行收缩
    if(mesh.is_boundary(eh) || mesh.is_boundary(to))
      return 0;
    // 检查是否可以做collapse操作，避免collapse时报错
    if(!mesh.is_collapse_ok(he_h))
      return 0;
    auto vbest = mesh.property(cost_v, eh); 
    mesh.set_point(to, vbest.optimalv3);
    mesh.collapse(he_h);
    mesh.garbage_collection();
    for(auto it=elist.begin(); it<elist.end(); it++){
      if(!mesh.is_valid_handle(*it))
        elist.erase(it);
    }
    return 1;
  }
#endif
void DeleteVertex(int total){
  // 获得删除的pair
  while(total && elist.size()){
    auto e_del = elist.front();
    elist.erase(elist.begin());
    if(updateElist(e_del))
      total--;
  }
  cout<<"Still have "<<total<<" vertex to delete"<<endl;
}
// 初始化mesh的性质
void MeshPropInit(){
  // 获取点边面的信息，用于后续顶点删除
  if(!mesh.has_face_status())
    mesh.request_face_status();
  if(!mesh.has_edge_status())
    mesh.request_edge_status();
  if(!mesh.has_vertex_status())
    mesh.request_vertex_status();
  // 加入自定义属性
  addQproperty();
  addItemproperty();
}

// mesh性质的回收
void MeshPropDel(){
  mesh.release_face_status();
  mesh.release_edge_status();
  mesh.release_vertex_status();
  mesh.remove_property(q);
  mesh.remove_property(cost_v);
}