#pragma once
#include <SDL.h>
#include "scene.h"
#include "v_shader.h"
#include "f_shader.h"

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
	std::vector<unsigned char> pixel_data;
	std::vector<unsigned char> click_map;
	
	int ScreenX(const ShadedVertex& coords) const;
	int ScreenY(const ShadedVertex& coords) const;
	static bool InBound(const int coord, const int max);

	// events
	void MouseButtonUp(SDL_MouseButtonEvent e);
};
