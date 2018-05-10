#pragma once

struct InputMapComponent {
	// Vehicle style input mappings
	int gamepadIdx;
	int turnAxisMap;
	int accelerationBtnMap;
	int brakeBtnMap;

	// Direct camera control input mappings
	bool mouseInputEnabled;
	int leftBtnMap;
	int rightBtnMap;
	int forwardBtnMap;
	int backwardBtnMap;
	int downBtnMap;
	int upBtnMap;
	int azimuthPosBtnMap;
	int azimuthNegBtnMap;
	int elevationPosBtnMap;
	int elevationNegBtnMap;
	int rollBtnMap;
};