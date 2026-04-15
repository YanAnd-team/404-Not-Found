#pragma once
#include "raylib.h"

enum Map { Vacum = 0, Wall1, Wall2, Wall3, Spike };

class Level
{
public:
	void Init();
	void Update();
	void DeInit();
	void Load();
	//
private:
	Texture2D Spike;
	Texture2D TrapSpike;
	Texture2D GunTrap;
};
