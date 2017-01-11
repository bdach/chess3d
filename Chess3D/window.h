#pragma once
#include <SDL.h>
#include "scene.h"
#include "v_shader.h"
#include "f_shader.h"

class Window
{
public:
	explicit Window(const std::string& _title);
	Window(const Window& _other) = delete;
	Window& operator=(const Window& _other) = delete;
	void Show(Scene& scene);
	void RenderScene(const Scene& scene);
	~Window();

private:
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* frame_buffer;
	std::vector<unsigned char> pixel_data;
	
	int ScreenX(const ShadedVertex& coords) const;
	int ScreenY(const ShadedVertex& coords) const;
	static bool InBound(const int coord, const int max);
};
