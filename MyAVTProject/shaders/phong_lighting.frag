#version 430

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

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

uniform int dirLightOn;
uniform DirLight dirLight;
uniform int pointLightsOn;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform int spotLightsOn;
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];

out vec4 colorOut;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform Materials mat;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} DataIn;

vec4 calculateDirLight(DirLight light, vec3 n, vec3 e);
vec4 calculatePointLight(PointLight light, vec3 n, vec3 e);
vec4 calculateSpotLight(SpotLight light, vec3 n, vec3 e);

void main()
{
	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(DataIn.eye);

	vec4 res = vec4(0.0);

	// add the directional light's contribution to the output
	if (dirLightOn == 1) {
		res += calculateDirLight(dirLight, n, e);
	}
		
	// do the same for all point lights
	if (pointLightsOn == 1) {
		for(int i = 0; i < NUM_POINT_LIGHTS; i++)
			res += calculatePointLight(pointLights[i], n, e);
	}
		
	// and add others lights as well (like spotlights)
	if (spotLightsOn == 1) {
		for(int i = 0; i < NUM_SPOT_LIGHTS; i++)
			res += calculateSpotLight(spotLights[i], n, e);
	}
	
	// check if the last paramater of output isn't wrong
	colorOut = max(res, mat.ambient);
}

vec4 calculateDirLight(DirLight light, vec3 n, vec3 e)
{
	vec4 spec = vec4(0.0);

	vec3 lightDir = normalize(vec3(light.direction)); //fragPos can be -eye
	float intensity = max(dot(n,lightDir), 0.0);

	if (intensity > 0.0) {
		vec3 h = normalize(lightDir + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);
	}
	
	return intensity * mat.diffuse + spec;
}  

vec4 calculatePointLight(PointLight light, vec3 n, vec3 e)
{
	vec4 spec = vec4(0.0);

	vec3 lightDir = normalize(vec3(light.position) + DataIn.eye);
    float intensity = max(dot(n,lightDir), 0.0);

	if (intensity > 0.0) {
		vec3 h = normalize(lightDir + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);
	}
	
	return intensity * mat.diffuse + spec;
}

vec4 calculateSpotLight(SpotLight light, vec3 n, vec3 e) // should be just position for now
{
	vec4 spec = vec4(0.0);

	vec3 lightDir = normalize(vec3(light.position) + DataIn.eye);
	float theta = dot(lightDir, normalize(vec3(-light.direction)));
    
	if(theta > light.cutOff) 
	{       
		float spotExp = 0.0;
		float att = pow(theta, spotExp);
		float intensity = max(dot(n,lightDir), 0.0) * att;

		if (intensity > 0.0) {
			vec3 h = normalize(lightDir + e);
			float intSpec = max(dot(h,n), 0.0);
			spec = mat.specular * pow(intSpec, mat.shininess) * att;
		}
		return intensity * mat.diffuse + spec;
	}

	return vec4(0.0);
} 
