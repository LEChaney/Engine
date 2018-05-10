//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School.
//
// File Name	: Clock.h
// Description	: Header file for the clock
// Author		: Lance Chaney
// Mail			: lance.cha7337@mediadesign.school.nz
//

#pragma once

namespace Clock {
	void update();
	float getDeltaTime();
	float getUIDeltaTime();
	float getTime();
	void setTimeDilation(double timeDilation);
}
