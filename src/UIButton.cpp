#include "UIButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

UIButton::UIButton(int id, SDL_Rect bounds, const char* text) : UIElement(UIElementType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

UIButton::~UIButton()
{

}

bool UIButton::Update(float dt)
{
	if (state != UIElementState::DISABLED)
	{
		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

			state = UIElementState::FOCUSED;

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				state = UIElementState::PRESSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = UIElementState::NORMAL;
		}

		//L16: TODO 4: Draw the button according the GuiControl State
		switch (state)
		{
		case UIElementState::DISABLED:
			Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
			break;
		case UIElementState::NORMAL:
			if (normalTex)
				Engine::GetInstance().render->DrawTexture(normalTex, bounds.x, bounds.y);
			else
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
			break;
		case UIElementState::FOCUSED:
		{
			animTimer += dt * animSpeed;

			Uint8 r = (Uint8)(128 + 127 * sin(animTimer));
			Uint8 g = (Uint8)(128 + 127 * sin(animTimer + 2.0f * M_PI / 3.0f));
			Uint8 b = (Uint8)(128 + 127 * sin(animTimer + 4.0f * M_PI / 3.0f));
			if (focusTex)
			{
				SDL_SetTextureColorMod(focusTex, r, g, b);
				Engine::GetInstance().render->DrawTexture(focusTex, bounds.x, bounds.y);
				SDL_SetTextureColorMod(focusTex, 255, 255, 255);
			}
			else
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
			break;
		}
		case UIElementState::PRESSED:
			if (pressedTex)
				Engine::GetInstance().render->DrawTexture(pressedTex, bounds.x, bounds.y);
			else
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
			break;
		}

		Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, {255,255,255,255});
	}

	return false;
}

bool UIButton::CleanUp()
{
	pendingToDelete = true;
	return true;
}

void UIButton::SetTextures(SDL_Texture* normal, SDL_Texture* focused, SDL_Texture* pressed)
{
	normalTex = normal;
	focusTex = focused ? focused : normal; 
	pressedTex = pressed ? pressed : normal;
}