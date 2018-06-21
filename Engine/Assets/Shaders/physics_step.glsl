#version 430 core

layout (local_size_x = 128) in;

uniform float Gravity1 = 10.0;
uniform vec3 BlackHolePos1 = vec3(0, 15, 0);
uniform float Gravity2 = 10.0;
uniform vec3 BlackHolePos2 = vec3(0, 0, 0);

uniform float ParticleInvMass = 1.0 / 0.1;
uniform float DeltaT = 1.0 / 60.0;

struct Particle {
	vec4 positionAndLifetime;
	vec4 velocity;
	vec4 initialPositionAndLifetime;
	vec4 initialVelocity;
};

layout (std430, binding=0) buffer Particles {
	Particle particle[];
};

void main() {
	uint idx = gl_GlobalInvocationID.x;
	//pointMasses[idx].position = vec4(0, float(idx) * 0.1f, 0, 1);
	
	vec3 p = particle[idx].positionAndLifetime.xyz;
	vec3 v = particle[idx].velocity.xyz;
	float lifetime = particle[idx].positionAndLifetime.w;
	
	// Force from black hole #1
	vec3 d = BlackHolePos1 - p;
	vec3 force = (Gravity1 / length(d)) * normalize(d);
	
	// Force from black hole #2
	d = BlackHolePos2 - p;
	force += (Gravity2 / length(d)) * normalize(d);
	
	// Apply simple Euler integrator
	vec3 a = force * ParticleInvMass;
	v = v + a * DeltaT;
	particle[idx].velocity = vec4(v, 0);
	particle[idx].positionAndLifetime = vec4(p + v * DeltaT + 0.5 * a * DeltaT * DeltaT, lifetime);

	// Update lifetime
	if (lifetime <= 0) {
		particle[idx].positionAndLifetime = particle[idx].initialPositionAndLifetime;
		particle[idx].velocity = particle[idx].initialVelocity;
	} else {
		particle[idx].positionAndLifetime.w -= DeltaT;
	}
}
