#pragma once

#include "BaseEnemy.h"

class Tomato : public BaseEnemy {
	Tomato();
	~Tomato();


	bool Awake();
	bool Start();
	bool Update(float dt);

	bool CleanUp();
	bool Destroy();


	void Draw(float dt);

};