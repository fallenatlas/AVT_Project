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
  glStencilMask(0xFF);  // Enable writing to the stencil buffer
  for (unsigned int i = 0; i < Nodes.size(); i++) {
    Nodes[i]->draw();
  }
  glStencilMask(0x00);
}

ScenegraphNode* Scenegraph::getNode(int id) {
    return Nodes[id];
}