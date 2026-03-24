#pragma once

#include "BaseNPC.h"

class NPC_Vagabundo : public BaseNPC {
public:

	NPC_Vagabundo();
	NPC_Vagabundo(int ID);
	~NPC_Vagabundo();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	bool Destroy();

};