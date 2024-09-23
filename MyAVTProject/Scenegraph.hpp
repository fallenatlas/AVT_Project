#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include "ScenegraphNode.hpp"
#include <vector>
#include <camera.h>

class Scenegraph;

////////////////////////////////////////////////////////////////////// Scenegraph

class Scenegraph {
  public:
    Scenegraph();
    Scenegraph(Camera* camera);
    ~Scenegraph();

    void addNode(ScenegraphNode* node);
    void draw();

    ScenegraphNode* getNode(int id);

  
  private:
    std::vector<ScenegraphNode*> Nodes;
    Camera* camera;
};
////////////////////////////////////////////////////////////////////////////////

#endif /* MGL_SCENEGRAPH_HPP */