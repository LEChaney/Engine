//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Per entity vehicleMovement settings.
// Author       : Lance Chaney Jack Mair
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

struct VehicleMovementComponent {
	float maxAcceleration = 1000.0f;    // This sets the maximum turning / acceleration speed
	float accelerationWeight = 1000.0f; // This scales how fast the entity will turn / accelerate
	float maxMoveSpeed = 1000.0f;
};