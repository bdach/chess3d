#include <exception>
#include "window.h"
#include "v_shader.h"

Window::Window(const std::string& _title)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::exception("Cannot initialize SDL");
	}
	window = SDL_CreateWindow(_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		throw std::exception("Window could not be created");
	}
	surface = SDL_GetWindowSurface(window);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		throw std::exception("Could not create renderer");
	}
}

void Window::Show(const Scene& scene) const
{
	SDL_RenderClear(renderer);
	SDL_UpdateWindowSurface(window);
	auto quit = false;
	SDL_Event event;
	RenderScene(scene);
	while (!quit) {
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}
}

void Window::RenderScene(const Scene& scene) const
{
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);

	for (auto mesh : scene.meshes)
	{
		std::vector<ShadedVertex> processed;
		auto back_inserter = std::back_inserter(processed);
		VertexShader::TransformCoords(mesh, scene.cameras[0], back_inserter);
		for (auto face : mesh.faces)
		{
			unsigned int count = face.indices.size();
			for (unsigned int i = 0; i < count; ++i)
			{
				SDL_RenderDrawLine(renderer,
					ScreenX(processed[face.indices[i]]), ScreenY(processed[face.indices[i]]),
					ScreenX(processed[face.indices[(i + 1) % count]]), ScreenY(processed[face.indices[(i + 1) % count]]));
			}
		}
	}

	SDL_RenderPresent(renderer);
}

Window::~Window()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
	SDL_DestroyRenderer(renderer);
}

int Window::ScreenX(const ShadedVertex& vertex) const
{
	return (vertex.coords.x() + 1) / 2 * SCREEN_WIDTH;
}

int Window::ScreenY(const ShadedVertex& vertex) const
{
	return (vertex.coords.y() - 1) / 2 * -SCREEN_HEIGHT;
}