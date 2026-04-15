#include "Level.h"

void Level::Init()
{
	Spike = LoadTexture("resources/Image/SpikeTrapWait.png");
	TrapSpike = LoadTexture("resources/Image/TrapSpike.png");
	GunTrap = LoadTexture("resources/Image/GunTrap.png");
}

void Level::Update()
{
	//
}

void Level::DeInit()
{
	UnloadTexture(Spike);
	UnloadTexture(TrapSpike);
	UnloadTexture(GunTrap);
}

void Level::Load()
{
	//
}