#include "ScenegraphNode.hpp"
#include "GL/glew.h"

////////////////////////////////////////////////////////////////////////////////

ScenegraphNode::ScenegraphNode() {}

ScenegraphNode::ScenegraphNode(SceneElement* element, VSShaderLib* shader, int shaderMode) {
    Element = element;
    Shader = shader;
	ShaderMode = shaderMode;
}

ScenegraphNode::~ScenegraphNode() {}

void ScenegraphNode::addNode(ScenegraphNode* node) {
    Children.resize(Children.size() + 1);
    Children[Children.size() - 1] = node;
    node->Parent = this;
}

void ScenegraphNode::aiRecursive_render(const aiNode* nd, std::vector<struct MyMesh>& myMeshes, GLuint*& textureIds)
{
	GLint loc;
	GLint pvm_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_pvm");
	GLint vm_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_viewModel");
	GLint normal_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_normal");
	GLint lPos_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "l_pos");
	GLint normalMap_loc = glGetUniformLocation(Shader->getProgramIndex(), "normalMap");
	GLint specularMap_loc = glGetUniformLocation(Shader->getProgramIndex(), "specularMap");
	GLint diffMapCount_loc = glGetUniformLocation(Shader->getProgramIndex(), "diffMapCount");

	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;
	// OpenGL matrices are column major
	m.Transpose();

	// save model matrix and apply node transformation
	pushMatrix(MODEL);

	float aux[16];
	memcpy(aux, &m, sizeof(float) * 16);
	multMatrix(MODEL, aux);

	// draw all meshes assigned to this node
	for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {

		// send the material
		loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.ambient);
		loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.diffuse);
		loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.specular);
		loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.emissive");
		glUniform4fv(loc, 1, myMeshes[nd->mMeshes[n]].mat.emissive);
		loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes[nd->mMeshes[n]].mat.shininess);
		loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.texCount");
		glUniform1i(loc, myMeshes[nd->mMeshes[n]].mat.texCount);

		unsigned int  diffMapCount = 0;  //read 2 diffuse textures

		//devido ao fragment shader suporta 2 texturas difusas simultaneas, 1 especular e 1 normal map

		glUniform1i(normalMap_loc, false);   //GLSL normalMap variable initialized to 0
		glUniform1i(specularMap_loc, false); //
		glUniform1ui(diffMapCount_loc, 0); // this is for knowing how many texture we want to use

		if (myMeshes[nd->mMeshes[n]].mat.texCount != 0)
			for (unsigned int i = 0; i < myMeshes[nd->mMeshes[n]].mat.texCount; ++i) {

				//Activate a TU with a Texture Object
				GLuint TU = myMeshes[nd->mMeshes[n]].texUnits[i];
				glActiveTexture(GL_TEXTURE0 + TU);
				glBindTexture(GL_TEXTURE_2D, textureIds[TU]);

				if (myMeshes[nd->mMeshes[n]].texTypes[i] == DIFFUSE) {
					if (diffMapCount == 0) {
						diffMapCount++;
						loc = glGetUniformLocation(Shader->getProgramIndex(), "texUnitDiff");
						glUniform1i(loc, TU);
						glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else if (diffMapCount == 1) {
						diffMapCount++;
						loc = glGetUniformLocation(Shader->getProgramIndex(), "texUnitDiff1");
						glUniform1i(loc, TU);
						glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else printf("Only supports a Material with a maximum of 2 diffuse textures\n");
				}
				else if (myMeshes[nd->mMeshes[n]].texTypes[i] == SPECULAR) {
					loc = glGetUniformLocation(Shader->getProgramIndex(), "texUnitSpec");
					glUniform1i(loc, TU);
					glUniform1i(specularMap_loc, true);
				}
				else if (myMeshes[nd->mMeshes[n]].texTypes[i] == NORMALS) { //Normal map
					loc = glGetUniformLocation(Shader->getProgramIndex(), "texUnitNormalMap");
					if (Element->normalMapKey)
						glUniform1i(normalMap_loc, Element->normalMapKey);
					glUniform1i(loc, TU);

				}
				else printf("Texture Map not supported\n");
			}

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// bind VAO
		glBindVertexArray(myMeshes[nd->mMeshes[n]].vao);

		/*
		if (!Shader->isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		*/
		// draw
		glDrawElements(myMeshes[nd->mMeshes[n]].type, myMeshes[nd->mMeshes[n]].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// draw all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		aiRecursive_render(nd->mChildren[n], myMeshes, textureIds);
	}
	popMatrix(MODEL);
}


void ScenegraphNode::draw(bool shadowMode, bool reflectionMode) {
    pushMatrix(MODEL);
    translate(MODEL, Element->translation);
    rotate(MODEL, Element->rotation);
    scale(MODEL, Element->scale);

	if (Element->usingAssimp) {
		// call airecursive_render
		aiRecursive_render(Element->scene->mRootNode, Element->meshes, Element->textureIds);
	}
	else {
		if (Element->mesh.isValid) {
			if (Element->mesh.mat.diffuse[3] == 0) glDepthMask(GL_FALSE);
			GLint loc;
			GLint pvm_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_pvm");
			GLint vm_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_viewModel");
			GLint normal_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "m_normal");
			GLint lPos_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "l_pos");
			GLint shaderMode_uniformId = glGetUniformLocation(Shader->getProgramIndex(), "shaderMode");
			GLint normalMap_loc = glGetUniformLocation(Shader->getProgramIndex(), "normalMap");
			GLint specularMap_loc = glGetUniformLocation(Shader->getProgramIndex(), "specularMap");
			GLint diffMapCount_loc = glGetUniformLocation(Shader->getProgramIndex(), "diffMapCount");

			// send the material
			loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, Element->mesh.mat.ambient);
			loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, Element->mesh.mat.diffuse);
			loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, Element->mesh.mat.specular);
			loc = glGetUniformLocation(Shader->getProgramIndex(), "mat.shininess");
			glUniform1f(loc, Element->mesh.mat.shininess);

			//devido ao fragment shader suporta 2 texturas difusas simultaneas, 1 especular e 1 normal map

			glUniform1i(normalMap_loc, Element->normalMapKey);   //GLSL normalMap variable initialized to 0
			glUniform1i(specularMap_loc, false); //
			glUniform1ui(diffMapCount_loc, 0); // this is for knowing how many texture we want to use

			if (Element->numTextures == 1) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Element->textureIds[0]);

				loc = glGetUniformLocation(Shader->getProgramIndex(), "texUnitDiff");
				glUniform1i(loc, 0);
				glUniform1ui(diffMapCount_loc, 1);
			}
			else if (Element->numTextures == 2) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Element->textureIds[0]);

				loc = glGetUniformLocation(Shader->getProgramIndex(), "texUnitDiff");
				glUniform1i(loc, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, Element->textureIds[1]);

				loc = glGetUniformLocation(Shader->getProgramIndex(), "texUnitDiff1");
				glUniform1i(loc, 1);
				glUniform1ui(diffMapCount_loc, 2);
			}

			// send matrices to OGL
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);
			glUniform1i(shaderMode_uniformId, ShaderMode);

			// Render mesh
			glBindVertexArray(Element->mesh.vao);

			glDrawElements(Element->mesh.type, Element->mesh.numIndexes, GL_UNSIGNED_INT, 0);
			if (Element->mesh.mat.diffuse[3] == 0) glDepthMask(GL_TRUE);
			glBindVertexArray(0);
		}
	}

    for(ScenegraphNode* node : Children) {
        node->draw(shadowMode, reflectionMode);
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
	Element->rotation[3] = rotation[3];
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