#include "Engine.h"
#include "Window.h"
#include "Render.h"
#include "Log.h"
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CHAR_LENGTH 10
#define CHAR_HEIGHT 16
#define TEXT_DISPLAY_X 200
#define TEXT_DISPLAY_Y 1000

#define MAX_CHARS_PER_LINE 75

std::string fullText;
int visibleChars = 0;
float typeTimer = 0.0f;
float typeSpeed = 0.05f;
bool isTyping = false;

int Length(const char* text) {
	int count = 0;
	while (text[count] != '\0') { count++; }
	return count;
}

Render::Render() : Module()
{
	name = "render";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{
}

// Called before render is available
bool Render::Awake()
{
	LOG("Create SDL rendering context");
	bool ret = true;
	float scale = Engine::GetInstance().window->GetScale();
	SDL_Window* window = Engine::GetInstance().window->window;

	//L05 TODO 5 - Load the configuration of the Render module
	
	// SDL3: no flags; create default renderer and set vsync separately
	renderer = SDL_CreateRenderer(window, nullptr);
	SDL_SetRenderScale(renderer, scale, scale);
	

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		if (configParameters.child("vsync").attribute("value").as_bool())
		{
			if (!SDL_SetRenderVSync(renderer, 1))
			{
				LOG("Warning: could not enable vsync: %s", SDL_GetError());
			}
			else
			{
				LOG("Using vsync");
			}
		}

		camera.w = Engine::GetInstance().window->GetBaseWidth();
		camera.h = Engine::GetInstance().window->GetBaseHeight();
		camera.x = 0;
		camera.y = 0;
	}

	//initialise the SDL_ttf library
	TTF_Init();

	//load a font into memory
	font = TTF_OpenFont("Assets/Fonts/arial.ttf", 25);

	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	if (!SDL_GetRenderViewport(renderer, &viewport))
	{
		LOG("SDL_GetRenderViewport failed: %s", SDL_GetError());
	}
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool Render::Update(float dt)
{
	if (isTyping)
	{
		typeTimer += dt;

		if (typeTimer >= typeSpeed)
		{
			typeTimer = 0.0f;

			if (visibleChars < fullText.size())
			{
				visibleChars++;
			}
			else
			{
				isTyping = false;
			}
		}
	}
	return true;
}

bool Render::PostUpdate()
{
	if (!fullText.empty())
	{
		std::string visible = fullText.substr(0, visibleChars);

		for (int i = 0; i < visible.size(); i += MAX_CHARS_PER_LINE)
		{
			std::string line = visible.substr(i, MAX_CHARS_PER_LINE);
			TextDisplay(line, 0, (i / MAX_CHARS_PER_LINE) * CHAR_HEIGHT);
		}
	}
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_SetRenderViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_SetRenderViewport(renderer, &viewport);
}

void Render::UpdateScale()
{
	float scale = Engine::GetInstance().window->GetScale();

	SDL_SetRenderScale(renderer, scale, scale);

}

// Blit to screen
bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	//int scale = Engine::GetInstance().window->GetScale();
	// SDL3 uses float rects for rendering
	SDL_FRect rect;
	rect.x = (float)((int)(camera.x) + x);
	rect.y = (float)((int)(camera.y) + y);

	if (section != NULL)
	{
		rect.w = (float)(section->w);
		rect.h = (float)(section->h);
	}
	else
	{
		float tw = 0.0f, th = 0.0f;
		if (!SDL_GetTextureSize(texture, &tw, &th))
		{
			LOG("SDL_GetTextureSize failed: %s", SDL_GetError());
			return false;
		}
		rect.w = tw;
		rect.h = th;
	}

	const SDL_FRect* src = NULL;
	SDL_FRect srcRect;
	if (section != NULL)
	{
		srcRect.x = (float)section->x;
		srcRect.y = (float)section->y;
		srcRect.w = (float)section->w;
		srcRect.h = (float)section->h;
		src = &srcRect;
	}

	SDL_FPoint* p = NULL;
	SDL_FPoint pivot;
	if (pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = (float)pivotX;
		pivot.y = (float)pivotY;
		p = &pivot;
	}

	// SDL3: returns bool; map to int-style check
	int rc = SDL_RenderTextureRotated(renderer, texture, src, &rect, angle, p, SDL_FLIP_NONE) ? 0 : -1;
	if (rc != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderTextureRotated error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	int scale = Engine::GetInstance().window->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_FRect rec;
	if (use_camera)
	{
		rec.x = (float)((int)(camera.x + rect.x));
		rec.y = (float)((int)(camera.y + rect.y));
		rec.w = (float)(rect.w);
		rec.h = (float)(rect.h);
	}
	else
	{
		rec.x = (float)(rect.x);
		rec.y = (float)(rect.y);
		rec.w = (float)(rect.w);
		rec.h = (float)(rect.h);
	}

	int result = (filled ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderRect(renderer, &rec)) ? 0 : -1;

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect/SDL_RenderRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	int scale = Engine::GetInstance().window->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	float X1, Y1, X2, Y2;

	if (use_camera)
	{
		X1 = (float)(camera.x + x1);
		Y1 = (float)(camera.y + y1);
		X2 = (float)(camera.x + x2);
		Y2 = (float)(camera.y + y2);
	}
	else
	{
		X1 = (float)(x1);
		Y1 = (float)(y1);
		X2 = (float)(x2);
		Y2 = (float)(y2);
	}

	int result = SDL_RenderLine(renderer, X1, Y1, X2, Y2) ? 0 : -1;

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderLine error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	//int scale = Engine::GetInstance().window->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_FPoint points[360];

	float factor = (float)M_PI / 180.0f;

	float cx = (float)((use_camera ? camera.x : 0) + x);
	float cy = (float)((use_camera ? camera.y : 0) + y);

	for (int i = 0; i < 360; ++i)
	{
		points[i].x = cx + (float)(radius * cos(i * factor));
		points[i].y = cy + (float)(radius * sin(i * factor));
	}

	result = SDL_RenderPoints(renderer, points, 360) ? 0 : -1;

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderPoints error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// L16: method DrawText to render text on screen. Uses SDL3_ttf
bool Render::DrawText(const char* text, int x, int y, int w, int h, SDL_Color color) const
{
	if (!font || !renderer || !text) {
		LOG("DrawText: invalid font/renderer/text");
		return false;
	}

	// Render the text to a surface
	// SDL3_ttf: length can be 0 for null-terminated strings
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, 0, color);
	if (!surface) {
		LOG("DrawText: TTF_RenderText_Solid failed: %s", SDL_GetError());
		return false;
	}

	// Create a texture from the surface
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		LOG("DrawText: SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
		SDL_DestroySurface(surface);
		return false;
	}

	// Optional but often needed when using alpha/text
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	// If w/h are 0, use the text’s natural size
	float fw = (w > 0) ? (float)w : (float)surface->w;
	float fh = (h > 0) ? (float)h : (float)surface->h;

	SDL_FRect dstrect = { (float)x, (float)y, fw, fh };

	// Render the texture to the current render target
	if (!SDL_RenderTexture(renderer, texture, nullptr, &dstrect)) {
		LOG("DrawText: SDL_RenderTexture failed: %s", SDL_GetError());
	}

	// Cleanup
	SDL_DestroyTexture(texture);
	SDL_DestroySurface(surface);

	return true;
}

bool Render::TextDisplay(std::string text, int x_offset, int y_offset) {
	const char* ctext = text.c_str();
	int w = Length(ctext)*CHAR_LENGTH; //width doesn't change proportionally as some chars have less spacing than others
	int h = CHAR_HEIGHT;
	SDL_Color color = { 0,0,0,0 };
	DrawText(ctext, TEXT_DISPLAY_X+x_offset, TEXT_DISPLAY_Y+y_offset, w, h, color);

	return true;
}

bool Render::AnimatedTextDisplay(const char* text) {
	/*int chars = Length(text);
	std::string displayed_text;
	for (int i = 1; i < chars; i++) {
		displayed_text = std::string(text).substr(0, i);
		TextDisplay(displayed_text);
	}
	*/
	return true;
}

void Render::StartTextDisplay(const char* text, float speed) {
	fullText = text;
	visibleChars = 0;
	typeTimer = 0.0f;
	typeSpeed = speed;
	isTyping = true;
}