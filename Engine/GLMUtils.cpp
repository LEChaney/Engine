//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "GLMUtils.h"

#include <array>

glm::vec3 ease(glm::vec3 t) {
                                                          // Fade function as defined by Ken Perlin.  This eases coordinate values
                                                          // so that they will ease towards integral values.  This ends up smoothing
                                                          // the final output.
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);  // 6t^5 - 15t^4 + 10t^3
}

float gradDot(int cubeCornerHash, glm::vec3 diplacementFromCorner)
{
	switch (cubeCornerHash & 0xF)
	{
	case 0x0: return  diplacementFromCorner.x + diplacementFromCorner.y;
	case 0x1: return -diplacementFromCorner.x + diplacementFromCorner.y;
	case 0x2: return  diplacementFromCorner.x - diplacementFromCorner.y;
	case 0x3: return -diplacementFromCorner.x - diplacementFromCorner.y;
	case 0x4: return  diplacementFromCorner.x + diplacementFromCorner.z;
	case 0x5: return -diplacementFromCorner.x + diplacementFromCorner.z;
	case 0x6: return  diplacementFromCorner.x - diplacementFromCorner.z;
	case 0x7: return -diplacementFromCorner.x - diplacementFromCorner.z;
	case 0x8: return  diplacementFromCorner.y + diplacementFromCorner.z;
	case 0x9: return -diplacementFromCorner.y + diplacementFromCorner.z;
	case 0xA: return  diplacementFromCorner.y - diplacementFromCorner.z;
	case 0xB: return -diplacementFromCorner.y - diplacementFromCorner.z;
	case 0xC: return  diplacementFromCorner.y + diplacementFromCorner.x;
	case 0xD: return -diplacementFromCorner.y + diplacementFromCorner.z;
	case 0xE: return  diplacementFromCorner.y - diplacementFromCorner.x;
	case 0xF: return -diplacementFromCorner.y - diplacementFromCorner.z;
	default: return 0; // never happens
	}
}

float GLMUtils::perlin(const glm::vec3& inputPos)
{
	static const std::array<int, 512> p = { 151,160,137,91,90,15,                       // Hash lookup table as defined by Ken Perlin.  This is a randomly
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    // arranged array of all numbers from 0-255 inclusive.
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

		// Doubled to avoid buffer overflow
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
	};

	// Calculate the "unit cube" that the point asked will be located in
	// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
	// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
	glm::ivec3 cubePos = { static_cast<int>(inputPos.x) & 255, static_cast<int>(inputPos.y) & 255, static_cast<int>(inputPos.z) & 255 };
	glm::vec3 fracPos = glm::fract(inputPos);

	// Hash all 8 unit cube coordinates
	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aaa = p[p[p[cubePos.x] + cubePos.y] + cubePos.z];
	aba = p[p[p[cubePos.x] + cubePos.y + 1] + cubePos.z];
	aab = p[p[p[cubePos.x] + cubePos.y] + cubePos.z + 1];
	abb = p[p[p[cubePos.x] + cubePos.y + 1] + cubePos.z + 1];
	baa = p[p[p[cubePos.x + 1] + cubePos.y] + cubePos.z];
	bba = p[p[p[cubePos.x + 1] + cubePos.y + 1] + cubePos.z];
	bab = p[p[p[cubePos.x + 1] + cubePos.y] + cubePos.z + 1];
	bbb = p[p[p[cubePos.x + 1] + cubePos.y + 1] + cubePos.z + 1];

	// Ease function for interpolating
	glm::vec3 easeFunc = ease(fracPos);

	// Use cubeCornerHash to look up gradient vector and perform dot product
	float x1, x2, y1, y2;
	x1 = glm::lerp(                                                       // The gradient function calculates the dot product between a pseudorandom
		gradDot(aaa, fracPos),                                            // gradient vector and the vector from the input coordinate to the 8 
		gradDot(baa, { fracPos.x - 1, fracPos.y, fracPos.z }),            // surrounding points in its unit cube. 
		easeFunc.x                                                        // This is all then lerped together as a sort of weighted average based on the eased (x,y,z) 
	);                                                                    // values we made earlier. 
	x2 = glm::lerp(
		gradDot(aba, { fracPos.x, fracPos.y - 1, fracPos.z }),
		gradDot(bba, { fracPos.x - 1, fracPos.y - 1, fracPos.z }),
		easeFunc.x
	);

	y1 = glm::lerp(x1, x2, easeFunc.y);

	x1 = glm::lerp(
		gradDot(aab, { fracPos.x, fracPos.y, fracPos.z - 1 }),
		gradDot(bab, { fracPos.x - 1, fracPos.y, fracPos.z - 1 }),
		easeFunc.x
	);
	x2 = glm::lerp(
		gradDot(abb, { fracPos.x, fracPos.y - 1, fracPos.z - 1 }),
		gradDot(bbb, { fracPos.z - 1, fracPos.y - 1, fracPos.z - 1 }),
		easeFunc.x
	);

	y2 = glm::lerp(x1, x2, easeFunc.y);

	return (glm::lerp(y1, y2, easeFunc.z) + 1) / 2;                       // For convenience we bind the result to 0 - 1 (theoretical min/max before is [-1, 1])
}

float GLMUtils::octavePerlin(const glm::vec3& inputPos, int octaves, float persistence)
{
	float total = 0;
	float frequency = 1;
	float amplitude = 1;
	float maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
	for (int i = 0; i < octaves; ++i) {
		total += perlin(inputPos * frequency) * amplitude;

		maxValue += amplitude;

		amplitude *= persistence;
		frequency *= 2;
	}

	return total / maxValue;
}
