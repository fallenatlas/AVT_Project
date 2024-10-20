#ifndef SCENEGRAPHNODE_HPP
#define SCENEGRAPHNODE_HPP

#include "geometry.h"
#include <vector>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "assimp/scene.h"

class ScenegraphNode;

////////////////////////////////////////////////////////////////////// ScenegraphNode
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];
extern float mNormal3x3[9];

class ScenegraphNode {
  public:
    ScenegraphNode();
    ScenegraphNode(SceneElement* element, VSShaderLib* shader, int texture);
    ~ScenegraphNode();
    std::vector<ScenegraphNode*> getChildren();

    void draw(bool shadowMode, bool reflectionMode);
    void aiRecursive_render(const aiNode* nd, std::vector<struct MyMesh>& myMeshes, GLuint*& textureIds);
    void move(std::vector<float> translation);
    void position(std::vector<float> position);
    void spin(std::vector<float> rotation);
    void setRotation(std::vector<float> rotation);
    void addNode(ScenegraphNode* node);
    
    SceneElement* Element;
    bool isActive = true;
  private:
    VSShaderLib* Shader;
    std::vector<ScenegraphNode*> Children;
    ScenegraphNode* Parent;
    int TextureMode;

};

#endif /* MGL_SCENEGRAPHNODE_HPP */