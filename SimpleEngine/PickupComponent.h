#pragma once

struct PickupComponent
{
	bool isActive;
	double respawnTimeOffset = 3.0;
	double respawnTimeStamp;
};