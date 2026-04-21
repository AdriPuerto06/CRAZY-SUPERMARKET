#pragma once

#include "UIElement.h"
#include "Vector2D.h"

class UIButton : public UIElement
{

public:

	UIButton(int id, SDL_Rect bounds, const char* text);
	virtual ~UIButton();

	// Called each loop iteration
	bool Update(float dt);
	bool CleanUp() override;

	void SetTextures(SDL_Texture* normal, SDL_Texture* focused = nullptr, SDL_Texture* pressed = nullptr);

private:

	bool canClick = true;
	bool drawBasic = false;

	SDL_Texture* normalTex = nullptr;
	SDL_Texture* focusTex = nullptr;
	SDL_Texture* pressedTex = nullptr;

	float animTimer = 0.0f;
	float animSpeed = 10.0f;
};

#pragma once