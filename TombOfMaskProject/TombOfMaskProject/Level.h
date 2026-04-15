#pragma once
#include "raylib.h"

enum Map { Empty='-', Wall1='1', Wall2='2', Wall3='3', Spike='S', TrapSpike='T', GunTrap='G' };

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
