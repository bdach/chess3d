#pragma once
#include <SDL.h>
#include "scene.h"
#include "v_shader.h"

class Window
{
public:
	explicit Window(const std::string& _title);
	Window(const Window& _other) = delete;
	Window& operator=(const Window& _other) = delete;
	void Show(const Scene& scene) const;
	void RenderScene(const Scene& scene) const;
	~Window();

private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;

	int ScreenX(const ShadedVertex& coords) const;
	int ScreenY(const ShadedVertex& coords) const;

	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Renderer* renderer;
};
