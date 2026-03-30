#pragma once
#include "raylib.h"

enum Map = { Void = 0,Wall1,Wall2,Wall3,Spike,Guntrap};

class Level
{
public:
	void Init();
	void Update();
	void DeInit();
	//
private:
	//
};
