//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : An observer interface for keyboard events.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//


#pragma once

class IKeyObserver {
public:
	// Handles keyboard events.
	virtual void keyCallback(int key, int scancode, int action, int mods) = 0;
};