#version 430

#define NUM_POINT_LIGHTS_VERT 6
#define NUM_SPOT_LIGHTS_VERT 2

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_Model;   //por causa do cubo para a skybox
uniform mat3 m_normal;

uniform vec4 l_pos;
uniform bool normalMap;

in vec4 position;
in vec4 normal, tangent, bitangent;   //por causa do gerador de geometria
in vec4 texCoord;

struct DirLight {
	vec4 direction;
};

struct PointLight {
	vec4 position;
};

struct SpotLight {
	vec4 position;
	vec4 direction;
	float cutOff;
};

uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS_VERT];
uniform SpotLight spotLights[NUM_SPOT_LIGHTS_VERT];

out Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
	vec2 tex_coord;
	vec3 viewEye;
	vec3 skyboxTexCoord;

	vec3 directionalLightDir; // compute these, maybe test if this works first
	vec3 pointLightsDir[NUM_POINT_LIGHTS_VERT];
	vec3 spotLightsDir[NUM_SPOT_LIGHTS_VERT];
	vec3 spotLigthsPointingDirection[NUM_SPOT_LIGHTS_VERT];
	float spotLightsCutOff[NUM_SPOT_LIGHTS_VERT];
} DataOut;

void main () {

	vec3 n, t, b;
	vec3 eyeDir;
	vec3 temp, aux;

	vec4 pos = m_viewModel * position;

	n = normalize(m_normal * normal.xyz);
	DataOut.lightDir = vec3(l_pos - pos);
	DataOut.viewEye = vec3(-pos);
	eyeDir =  vec3(-pos);

	DataOut.skyboxTexCoord = vec3(m_Model * position);
	DataOut.skyboxTexCoord.x = - DataOut.skyboxTexCoord.x; //Texturas mapeadas no interior logo negar a coordenada x
	DataOut.tex_coord = texCoord.st;

	if (normalMap) {  //transform eye and light vectors by tangent basis
		t = normalize(m_normal * tangent.xyz);
		b = normalize(m_normal * bitangent.xyz);

		temp = vec3(dirLight.direction);
		aux.x = dot(temp, t);
		aux.y = dot(temp, b);
		aux.z = dot(temp, n);
		DataOut.directionalLightDir = normalize(aux);

		for (int i = 0; i < NUM_POINT_LIGHTS_VERT; i++) {
			temp = vec3(pointLights[i].position - pos);
			aux.x = dot(temp, t);
			aux.y = dot(temp, b);
			aux.z = dot(temp, n);
			DataOut.pointLightsDir[i] = aux; // try not normalizing
		}

		for (int i = 0; i < NUM_SPOT_LIGHTS_VERT; i++) {
			temp = vec3(spotLights[i].position - pos);
			aux.x = dot(temp, t);
			aux.y = dot(temp, b);
			aux.z = dot(temp, n);
			DataOut.spotLightsDir[i] = normalize(aux);

			temp = vec3(spotLights[i].direction);
			aux.x = dot(temp, t);
			aux.y = dot(temp, b);
			aux.z = dot(temp, n);
			DataOut.spotLigthsPointingDirection[i] = normalize(aux);
			DataOut.spotLightsCutOff[i] = spotLights[i].cutOff;
		}
		aux.x = dot(eyeDir, t);
		aux.y = dot(eyeDir, b);
		aux.z = dot(eyeDir, n);
		eyeDir = normalize(aux);
	}
	else {
		DataOut.directionalLightDir = vec3(dirLight.direction);

		for(int i = 0; i < NUM_POINT_LIGHTS_VERT; i++) {
			DataOut.pointLightsDir[i] = vec3(pointLights[i].position - pos);
		}

		for(int i = 0; i < NUM_SPOT_LIGHTS_VERT; i++) {
			DataOut.spotLightsDir[i] = vec3(spotLights[i].position - pos);
			DataOut.spotLigthsPointingDirection[i] = vec3(spotLights[i].direction);
			DataOut.spotLightsCutOff[i] = spotLights[i].cutOff;
		}
	}

	DataOut.normal = n;
	DataOut.eye = eyeDir;

	gl_Position = m_pvm * position;	
}
