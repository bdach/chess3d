#include <exception>
#include "window.h"
#include "v_shader.h"

Window::Window(const std::string& _title) : pixel_data(SCREEN_WIDTH * SCREEN_HEIGHT * 4, 0)
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
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		throw std::exception("Could not create renderer");
	}
	frame_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (frame_buffer == nullptr)
	{
		throw std::exception("Could not create framebuffer texture");
	}
}

void Window::Show(const Scene& scene)
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

void Window::RenderScene(const Scene& scene)
{
	SDL_RenderPresent(renderer);

	FragmentShader fragment_shader(SCREEN_WIDTH, SCREEN_HEIGHT, pixel_data);
	for (auto mesh : scene.meshes)
	{
		std::vector<ShadedVertex> processed;
		auto back_inserter = std::back_inserter(processed);
		VertexShader::TransformCoords(mesh, scene.cameras[0], back_inserter);
		fragment_shader.Paint(mesh, processed);
	}
	SDL_UpdateTexture(frame_buffer, nullptr, &pixel_data[0], SCREEN_WIDTH * 4);
	SDL_RenderCopy(renderer, frame_buffer, nullptr, nullptr);
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
	return (vertex.ScreenX() + 1) / 2 * SCREEN_WIDTH;
}

int Window::ScreenY(const ShadedVertex& vertex) const
{
	return (vertex.ScreenY() - 1) / 2 * -SCREEN_HEIGHT;
}

bool Window::InBound(const int coord, const int max)
{
	return coord >= 0 && coord < max;
}
