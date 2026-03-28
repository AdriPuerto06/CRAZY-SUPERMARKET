#pragma once

#include "Module.h"
#include <SDL3/SDL.h>
#include "Vector2D.h"
class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* title);

	// Retrive window size
	void SetWindowSize(int& width, int& height) const;
	Vector2D GetWindowSize();
	// Retrieve window scale
	int GetScale() const;
	float GetBaseWidth() const { return baseWidth; }
	float GetBaseHeight() const { return baseHeight; }
	bool SetFullSize();
	bool SetWindowed(int scaleFactor);

public:
	// The window we'll be rendering to
	SDL_Window* window;

	std::string title;

	//this is for keeping the originar resolution
	int baseWidth;
	int baseHeight;

	int width = 0;
	int height = 0;
	int scale = 1;
};
