#pragma once
#include "Event.h"
//#include "Module.h"

//struct EventData {
//	const char* name;
//	int ID;
//};

class EventManager : public Module {
public:

	EventManager();
	virtual ~EventManager();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	std::shared_ptr<Event> GetEvent(const char* name);

	void GetEvents();

	bool IsEventActive(const char* name);
	bool IsEventActivated(const char* name);
	void PossibleActivate(const char* name);
	void MakeAction(const char* name);
	/*void ActivateEvent(const char* name);*/

private:
	std::vector<std::shared_ptr<Event>> events;

};