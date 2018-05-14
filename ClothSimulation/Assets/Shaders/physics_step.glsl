#version 430 core

layout (local_size_x = 1) in;

uniform float Gravity1 = 1000.0;
uniform vec3 BlackHolePos1 = vec3(0, 100, 0);
uniform float Gravity2 = 1000.0;
uniform vec3 BlackHolePos2 = vec3(0, 0, 0);

uniform float ParticleInvMass = 1.0 / 0.1;
uniform float DeltaT = 0.0005;

struct PointMass {
	vec4 position;
	vec4 velocity;
	vec4 force;
	float mass;
};

layout (std430, binding=0) buffer PointMasses {
	PointMass pointMasses[];
};

void main() {
	uint idx = gl_GlobalInvocationID.x;
	//pointMasses[idx].position = vec4(0, float(idx) * 0.1f, 0, 1);
	
	vec3 p = pointMasses[idx].position.xyz;
	vec3 v = pointMasses[idx].velocity.xyz;
	
	// Force from black hole #1
	vec3 d = BlackHolePos1 - p;
	vec3 force = (Gravity1 / length(d)) * normalize(d);
	
	// Force from black hole #2
	d = BlackHolePos2 - p;
	force += (Gravity2 / length(d)) * normalize(d);
	
	// Apply simple Euler integrator
	vec3 a = force * ParticleInvMass;
	pointMasses[idx].position = vec4(
	      p + v * DeltaT + 0.5 * a * DeltaT * DeltaT, 1.0);
	pointMasses[idx].velocity = vec4( v + a * DeltaT, 0.0);
}
