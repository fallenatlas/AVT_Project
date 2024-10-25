#version 430

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

uniform	sampler2D texUnitDiff;
uniform	sampler2D texUnitDiff1;
uniform	sampler2D texUnitSpec;
uniform	sampler2D texUnitNormalMap;
uniform samplerCube cubeMap;

uniform int shaderMode;
uniform int fogOn;
uniform int dirLightOn;
uniform int pointLightsOn;
uniform int spotLightsOn;

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
	vec2 tex_coord;
	vec3 viewEye;
	vec3 skyboxTexCoord;

	vec3 directionalLightDir;
	vec3 pointLightsDir[NUM_POINT_LIGHTS];
	vec3 spotLightsDir[NUM_SPOT_LIGHTS];
	vec3 spotLigthsPointingDirection[NUM_SPOT_LIGHTS];
	float spotLightsCutOff[NUM_SPOT_LIGHTS];
} DataIn;

uniform bool normalMap;
uniform bool specularMap;
uniform uint diffMapCount;

uniform bool shadowMode;

uniform mat4 m_View;

vec4 diff, auxSpec;
vec4 diffSum, specSum;

const float reflect_factor = 0.9;

// Point light attenuation parameters
const float constant = 1.0;
const float linear = 0.07;
const float quadratic = 0.017;

void calculateDirLight(vec3 lightDir, vec3 n, vec3 e);
void calculatePointLight(vec3 lightDir, vec3 n, vec3 e);
void calculateSpotLight(vec3 lightDir, vec3 pointingDir, float cutOff, vec3 n, vec3 e);

void main()
{
	if (mat.diffuse.a == 0.0) discard;
	
	if (shadowMode) {  //constant color
		vec3 res = vec3(0.5, 0.5, 0.5);
		// Fog calculation
		if (fogOn == 1) {
			vec3 fogColor = vec3(0.7, 0.7, 0.7);
			float distance = length(DataIn.viewEye);
			float fogDensity = 0.001;
			float fogFactor = exp(-fogDensity * distance* distance);
			fogFactor = clamp(fogFactor, 0.0, 1.0);
			res = mix(fogColor, res, fogFactor);
		}
		colorOut = vec4(res, mat.diffuse.a);
	}
	else if (shaderMode == 3) { // Lens flare
		if (fogOn == 1)
			colorOut = vec4(0); //Don't show flare if fog on
		else
			colorOut = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord);
	}
	else {
		vec3 n;
		if (normalMap)
			n = normalize(2.0 * texture(texUnitNormalMap, DataIn.tex_coord).rgb - 1.0);  //normal in tangent space
		else
			n = normalize(DataIn.normal);

		vec3 e = normalize(DataIn.eye);

		vec3 res = vec3(0.0);

		// determine diff color and spec color
		if (diffMapCount == 0)
			diff = mat.diffuse;
		else if(diffMapCount == 1) {
			diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord);
			if (diff.a == 0) discard;
		}
		else
			diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord) * texture(texUnitDiff1, DataIn.tex_coord);

		if (specularMap) 
			auxSpec = mat.specular * texture(texUnitSpec, DataIn.tex_coord);
		else
			auxSpec = mat.specular;

		// add the directional light's contribution to the output
		if (dirLightOn == 1) {
			calculateDirLight(DataIn.directionalLightDir, n, e);
		}
		
		// do the same for all point lights
		if (pointLightsOn == 1) {
			for(int i = 0; i < NUM_POINT_LIGHTS; i++)
				calculatePointLight(DataIn.pointLightsDir[i], n, e);
		}
		
		// and add others lights as well (like spotlights)
		if (spotLightsOn == 1) {
			for(int i = 0; i < NUM_SPOT_LIGHTS; i++)
				calculateSpotLight(DataIn.spotLightsDir[i], DataIn.spotLigthsPointingDirection[i], DataIn.spotLightsCutOff[i], n, e);
		}
	
		res = max(diffSum.rgb, diff.rgb*0.15) + specSum.rgb;

		if (shaderMode == 1) //SkyBox
			res = texture(cubeMap, DataIn.skyboxTexCoord).rgb;

		if (shaderMode == 2) { //Cube env mapping
			vec4 spec = vec4(0.0);
			vec3 l = normalize(DataIn.directionalLightDir);
			float intensity = max(dot(n,l), 0.0);

			if (intensity > 0.0) {
				vec3 h = normalize(l + e);
				float intSpec = max(dot(h,n), 0.0);
				spec = auxSpec * pow(intSpec, mat.shininess);
			}

			vec4 texel, texel1, cube_texel;
			vec3 reflected1 = vec3 (transpose(m_View) * vec4 (vec3(reflect(-e, n)), 0.0)); //reflection vector in world coord
			reflected1.x= -reflected1.x;   
			cube_texel = texture(cubeMap, reflected1);
			texel = texture(texUnitDiff, DataIn.tex_coord);  // texel from lighwood.tga
			vec4 aux_color = mix(texel, cube_texel, reflect_factor);
			aux_color = max(intensity*aux_color + spec, 0.1*aux_color);
			res = aux_color.rgb;
		}

		// Fog calculation
		if (fogOn == 1) {
			vec3 fogColor = vec3(0.7, 0.7, 0.7);
			float distance = length(DataIn.viewEye);
			float fogDensity = 0.001;
			float fogFactor = exp(-fogDensity * distance* distance);
			fogFactor = clamp(fogFactor, 0.0, 1.0);
			res = mix(fogColor, res, fogFactor);
		}

		colorOut = vec4(res, diff.a);
	}
}

void calculateDirLight(vec3 lightDir, vec3 n, vec3 e) {
	vec4 spec = vec4(0.0);

	vec3 l = normalize(lightDir); //fragPos can be -eye
	float intensity = max(dot(n,l), 0.0);

	if (intensity > 0.0) {
		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = auxSpec * pow(intSpec, mat.shininess);
	}

	diffSum += intensity * diff;
	specSum += spec;
}  

void calculatePointLight(vec3 lightDir, vec3 n, vec3 e) {
	vec4 spec = vec4(0.0);

	float dist = length(lightDir);
	float attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist)); // Constants to be tuned

	vec3 l = normalize(lightDir);
    float intensity = max(dot(n,l), 0.0);

	if (intensity > 0.0) {
		vec3 h = normalize(l + e);
		float intSpec = max(dot(h,n), 0.0);
		spec = auxSpec * pow(intSpec, mat.shininess);
	}
	
	diffSum += attenuation * intensity * diff;
	specSum += attenuation * spec;
}

void calculateSpotLight(vec3 lightDir, vec3 pointingDir, float cutOff, vec3 n, vec3 e) {
	vec4 spec = vec4(0.0);
	vec4 texel, texel1; 

	vec3 l = normalize(lightDir);
	float theta = dot(l, normalize(vec3(-pointingDir)));
    
	if(theta > cutOff) 
	{       
		float spotExp = 0.0;
		float att = pow(theta, spotExp);
		float intensity = max(dot(n,l), 0.0) * att;

		if (intensity > 0.0) {
			vec3 h = normalize(l + e);
			float intSpec = max(dot(h,n), 0.0);
			spec = auxSpec * pow(intSpec, mat.shininess) * att;
		}

		diffSum += intensity * diff;
		specSum += spec;
	}
} 
