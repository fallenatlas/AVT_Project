#include "Scenegraph.hpp"
#include <camera.h>

Scenegraph::Scenegraph() {}

Scenegraph::Scenegraph(Camera* c) {
    camera = c;
}

Scenegraph::~Scenegraph() {}

void Scenegraph::addNode(ScenegraphNode* node) {
    Nodes.resize(Nodes.size() + 1);
    Nodes[Nodes.size() - 1] = node;
}

void Scenegraph::draw(bool shadowMode, bool reflectionMode) {
 
  for (unsigned int i = 0; i < Nodes.size(); i++) {
      if (Nodes[i]->isActive) {
          loadIdentity(MODEL);

          if (shadowMode) {
              float mat[16];
              GLfloat plano_chao[4] = { 0,1,0,2 };
              float pointLightPos[] = { -100.0F, 300.0F, 100.0F, 1.0F };
              shadow_matrix(mat, plano_chao, pointLightPos);
              multMatrix(MODEL, mat);
          }
          else if (reflectionMode) {
              //pushMatrix(MODEL);
              scale(MODEL, -1.0f, 1.0f, 1.0f);
          }
          Nodes[i]->draw(shadowMode, reflectionMode);
      }
  }
  
}

ScenegraphNode* Scenegraph::getNode(int id) {
    return Nodes[id];
}