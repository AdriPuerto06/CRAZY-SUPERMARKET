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

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

private:

	void Draw(float dt);

};