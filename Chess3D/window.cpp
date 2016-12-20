#include <exception>
#include "window.h"

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
}

void Window::Show()
{
	SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));
	SDL_UpdateWindowSurface(window);
	SDL_Delay(5000);
}

Window::~Window()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}
