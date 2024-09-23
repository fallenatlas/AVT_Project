#ifndef SCENEGRAPHNODE_HPP
#define SCENEGRAPHNODE_HPP

#include "geometry.h"
#include <vector>
#include "VSShaderlib.h"
#include "AVTmathLib.h"

class ScenegraphNode;

////////////////////////////////////////////////////////////////////// ScenegraphNode
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];
extern float mNormal3x3[9];

class ScenegraphNode {
  public:
    ScenegraphNode();
    ScenegraphNode(int id, SceneElement* element, VSShaderLib* shader);
    ~ScenegraphNode();
    std::vector<float> getRotationMatrix();
    std::vector<float> getTranslationVector();
    std::vector<ScenegraphNode*> getChildren();

    void draw();
    void move(std::vector<float> translation);
    void spin(std::vector<float> rotation);
    void addNode(ScenegraphNode* node);
    
    SceneElement* Element;
  private:
    VSShaderLib* Shader;
    std::vector<ScenegraphNode*> Children;
    ScenegraphNode* Parent;
    int ObjectId;

};

#endif /* MGL_SCENEGRAPHNODE_HPP */