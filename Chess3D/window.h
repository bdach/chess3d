#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "scene.h"
#include "v_shader.h"
#include "f_shader.h"
#include "window_state.h"

class WindowMessage
{
public:
	WindowMessage(SDL_Renderer* renderer, SDL_Window* window);
	~WindowMessage();

	void SetText(std::string text);
	void SetColor(uint8_t r, uint8_t g, uint8_t b);
	SDL_Texture* GetTexture() const;
	void Hide();
	bool IsVisible() const;
	int Width() const;
	int Height() const;
private:
	TTF_Font* font;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Surface* surface;
	bool visible;
	std::string text;
	SDL_Color color;

	void CleanUp() const;
	void RenderText(std::string text);
};

enum CameraNumber
{
	STATIC_CAMERA = 0,
	FOLLOW_CAMERA = 1,
	OBJECT_CAMERA = 2
};

class Window
{
public:
	Window(const std::string& _title, Scene& scnee);
	Window(const Window& _other) = delete;
	Window& operator=(const Window& _other) = delete;
	void Show();
	void RenderScene(const Scene& scene);
	void RenderClickMap(const Scene& scene);
	~Window();

private:
	const int SCREEN_WIDTH = 1024;
	const int SCREEN_HEIGHT = 768;

	Scene& scene;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* frame_buffer;
	WindowMessage* message;
	StateManager state_manager;
	std::vector<unsigned char> pixel_data;
	std::vector<unsigned char> click_map;
	
	int ScreenX(const ShadedVertex& coords) const;
	int ScreenY(const ShadedVertex& coords) const;
	static bool InBound(const int coord, const int max);

	// events
	void MouseButtonUp(SDL_MouseButtonEvent e);
};