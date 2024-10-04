#include "ScenegraphNode.hpp"
#include "GL/glew.h"

////////////////////////////////////////////////////////////////////////////////

ScenegraphNode::ScenegraphNode() {}

ScenegraphNode::ScenegraphNode(SceneElement* element, VSShaderLib* shader, int texture) {
    Element = element;
    Shader = shader;
    TextureMode = texture;
}

ScenegraphNode::~ScenegraphNode() {}

void ScenegraphNode::addNode(ScenegraphNode* node) {
    Children.resize(Children.size() + 1);
    Children[Children.size() - 1] = node;
    node->Parent = this;
}

void ScenegraphNode::draw() {

    pushMatrix(MODEL);
    translate(MODEL, Element->translation);
    rotate(MODEL, Element->rotation);
    scale(MODEL, Element->scale);

    if (Element->mesh.isValid) {
        GLint loc;
        GLint pvm_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_pvm");
        GLint vm_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_viewModel");
        GLint normal_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_normal");
        GLint lPos_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "l_pos");
        GLint texMode_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "texMode"); // different modes of texturing

        // send the material
        loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.ambient");
        glUniform4fv(loc, 1, Element->mesh.mat.ambient);
        loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.diffuse");
        glUniform4fv(loc, 1, Element->mesh.mat.diffuse);
        loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.specular");
        glUniform4fv(loc, 1, Element->mesh.mat.specular);
        loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.shininess");
        glUniform1f(loc, Element->mesh.mat.shininess);

        // send matrices to OGL
        computeDerivedMatrix(PROJ_VIEW_MODEL);
        glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
        glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
        computeNormalMatrix3x3();
        glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);
        glUniform1i(texMode_uniformId, TextureMode);

        // Render mesh
        glBindVertexArray(Element->mesh.vao);

        glDrawElements(Element->mesh.type, Element->mesh.numIndexes, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    for(ScenegraphNode* node : Children) {
        node->draw();
    }

    memcpy(Element->mesh.transform, mMatrix[MODEL], 16 * sizeof(float));
    popMatrix(MODEL);
}

void ScenegraphNode::move(std::vector<float> translation) {
    Element->translation[0] += translation[0];
    Element->translation[1] += translation[1];
    Element->translation[2] += translation[2];
}

void ScenegraphNode::position(std::vector<float> position) {
    Element->translation[0] = position[0];
    Element->translation[1] = position[1];
    Element->translation[2] = position[2];
}

void ScenegraphNode::setRotation(std::vector<float> rotation) {
    Element->rotation[0] = rotation[0];
    Element->rotation[1] = rotation[1];
    Element->rotation[2] = rotation[2];
}

void ScenegraphNode::spin(std::vector<float> rotation) {
    Element->rotation[0] += rotation[0];
    Element->rotation[1] += rotation[1];
    Element->rotation[2] += rotation[2];
    Element->rotation[3] += rotation[3];
}

std::vector<ScenegraphNode*> ScenegraphNode::getChildren() {
    return Children;
}