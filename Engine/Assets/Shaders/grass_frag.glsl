#version 420 core

in VertexData {
	vec3 normal;
	vec2 texCoord;
	vec3 viewDir;
	vec3 worldPos;
} i;

out vec4 outColor;

layout (std140) uniform UniformBlock {
    mat4 model;
    mat4 view;
    mat4 projection;
	vec4 cameraPos;
	vec4 spotlightPositions[8];
	vec4 spotlightDirections[8];
	vec4 spotlightColors[8];
	uint numSpotlights;
	float metallicness;
	float glossiness;
	float specBias;
	float time;
	bool discardTransparent;
} u;

uniform sampler2D texSampler0;
uniform samplerCube radianceSampler;
uniform samplerCube irradianceSampler;

vec3 lightDir = vec3(1, 1, -1);
const vec3 LiDirect = vec3(1.28, 0.78, 0.62);
const int pmremMipCount = 11;
const float spotlightCuttoff = 0.5;
const float spotPow = 4;
const float attenuationScale = 0.01;

vec3 fresnel(vec3 specColor, vec3 lightDir, vec3 halfVector)
{
	return specColor + (1.0f - specColor) * pow(1.0f - clamp(dot(lightDir, halfVector), 0, 1), 5);
}

vec3 fresnelWithGloss(vec3 specColor, vec3 lightDir, vec3 halfVector, float gloss)
{
    return specColor + (max(vec3(gloss, gloss, gloss), specColor) - specColor) * pow(1.0f - clamp(dot(lightDir, halfVector), 0, 1), 5);
}

vec3 calcLrSpotlight(vec3 Cdiff, vec3 Cspec, vec3 normal, vec3 viewDir, vec3 lightPos, vec3 spotlightDir, vec3 lightCol, float specPow, float specNorm) 
{
	vec3 lightDisplacement = lightPos - i.worldPos;
	vec3 lightDir = normalize(lightDisplacement);
	spotlightDir = normalize(spotlightDir);
	float spotEffect = max(dot(-lightDir, spotlightDir), 0.0);
	if (spotEffect > spotlightCuttoff) {
		spotEffect = pow(spotEffect, spotPow);
		float attenuation = 1.0 / (1.0 + attenuationScale * pow(length(lightDisplacement), 2));
		vec3 halfVector = normalize(lightDir + viewDir);
		float ndotl = clamp(dot(lightDir, normal), 0, 1);
		float ndoth = clamp(dot(normal, halfVector), 0, 1);

		vec3 Fspec = fresnel(Cspec, lightDir, halfVector);
		vec3 Fdiff = Cdiff * (1 - Fspec) / (1.0000001 - Cspec);

		vec3 BRDFspec = specNorm * Fspec * pow(ndoth, specPow);
		return lightCol * spotEffect * attenuation * (Fdiff + BRDFspec) * ndotl;
	} else {
		return vec3(0, 0, 0);
	}	
}

void main(void)
{
	vec3 normal;
	if (gl_FrontFacing)
		normal = normalize(i.normal);
	else
		normal = -normalize(i.normal);

	vec4 color = texture(texSampler0, i.texCoord);

	if (u.discardTransparent && color.a < 0.5f)
		discard;

	// Direct Lighting variables
	vec3 viewDir = normalize(i.viewDir);
	lightDir = normalize(lightDir);
	vec3 halfVector = normalize(lightDir + viewDir);
	float ndotl = clamp(dot(lightDir, normal), 0, 1);
	float ndoth = clamp(dot(normal, halfVector), 0, 1);

	// Reflection variables
	float specPow = exp2(10 * u.glossiness + 1);
	float specNorm = (specPow + 8) / 8;
	float mipmapIndex = (1 - u.glossiness) * (pmremMipCount - 1); 
	vec3 LiReflDir = normalize(reflect(-viewDir, normal)); // The light direction that reflects directly into the camera
	vec3 LiRefl = textureLod(radianceSampler, LiReflDir, mipmapIndex).rgb;
	vec3 LiIrr = texture(irradianceSampler, normal).rgb;

	vec3 Cspec = mix(vec3(0.04, 0.04, 0.04) + u.specBias, color.rgb, u.metallicness);
	vec3 Cdiff = mix(vec3(0, 0, 0), color.rgb, 1 - u.metallicness);
	vec3 Fspec = fresnel(Cspec, lightDir, halfVector);
	vec3 Fdiff = Cdiff * (1 - Fspec) / (1.0000001 - Cspec);
	vec3 FspecRefl = fresnelWithGloss(Cspec, LiReflDir, normal, u.glossiness);
	vec3 FdiffRefl = Cdiff * (1 - FspecRefl) / (1.0000001 - Cspec);

	vec3 BRDFspec = specNorm * Fspec * pow(ndoth, specPow);

	vec3 LrSpotlight = vec3(0, 0, 0);
	for (uint i = 0; i < u.numSpotlights; ++i) {
		LrSpotlight += calcLrSpotlight(Cdiff, Cspec, normal, viewDir, u.spotlightPositions[i].xyz, u.spotlightDirections[i].xyz, u.spotlightColors[i].rgb, specPow, specNorm);
	}
	vec3 LrDirect = LiDirect * (Fdiff * 0.5f + BRDFspec) * ndotl;
	vec3 LrSubsurface = LiDirect * Fdiff * 0.5f;
	vec3 LrAmbDiff= LiIrr * FdiffRefl;
	vec3 LrAmbSpec = LiRefl * FspecRefl;

	outColor = vec4(LrDirect + LrSubsurface + LrSpotlight + LrAmbDiff + LrAmbSpec, color.a);
}
