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

void Scenegraph::draw() {
 
  for (unsigned int i = 0; i < Nodes.size(); i++) {
      if (Nodes[i]->isActive) {
          loadIdentity(MODEL);
          Nodes[i]->draw();
      }
  }
  
}

ScenegraphNode* Scenegraph::getNode(int id) {
    return Nodes[id];
}