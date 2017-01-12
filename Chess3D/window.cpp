#include "window.h"
#include "v_shader.h"
#include <iostream>
#include <set>

WindowMessage::WindowMessage(SDL_Renderer* renderer, SDL_Window* window) : renderer(renderer)
{
	if (TTF_Init() < 0)
	{
		throw std::exception("Could not init TTF");
	}
	texture = nullptr;
	surface = SDL_GetWindowSurface(window);
	if (surface == nullptr)
	{
		throw std::exception("Could not init surface");
	}
	color = { 0xff, 0xff, 0xff };
	visible = false;
	font = TTF_OpenFont("iosevka-medium.ttf", 24);
}


WindowMessage::~WindowMessage()
{
	CleanUp();
	if (surface != nullptr)
	{
		SDL_FreeSurface(surface);
	}
	if (font != nullptr)
	{
		TTF_CloseFont(font);
	}
	TTF_Quit();
}

void WindowMessage::SetText(std::string text)
{
	CleanUp();
	RenderText(text);
}

void WindowMessage::SetColor(uint8_t r, uint8_t g, uint8_t b)
{
	color = { r, g, b };
}

SDL_Texture* WindowMessage::GetTexture() const
{
	return texture;
}

void WindowMessage::Hide()
{
	visible = false;
}

bool WindowMessage::IsVisible() const
{
	return visible;
}

int WindowMessage::Width() const
{
	return surface->w;
}

int WindowMessage::Height() const
{
	return surface->h;
}

void WindowMessage::CleanUp() const
{
	if (texture != nullptr)
	{
		SDL_DestroyTexture(texture);
	}
}

void WindowMessage::RenderText(std::string text)
{
	visible = true;
	surface = TTF_RenderText_Solid(font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
}

Window::Window(const std::string& _title, Scene& scene) : pixel_data(SCREEN_WIDTH * SCREEN_HEIGHT * 4, 0), click_map(SCREEN_WIDTH * SCREEN_HEIGHT, 0xff), scene(scene)
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
	message = new WindowMessage(renderer, window);
}

void Window::Show()
{
	SDL_RenderClear(renderer);
	SDL_UpdateWindowSurface(window);
	PrepareCameras();
	RenderScene(true);
	auto quit = false;
	SDL_Event event;
	while (!quit) {
		state_manager.AdvanceStates();
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEBUTTONUP:
				MouseButtonUp(event.button);
				break;
			case SDL_KEYDOWN:
				KeyDown(event.key);
				break;
			default:
				break;
			}
		}
		if (state_manager.NextFrame())
		{
			FollowTarget(state_manager.FollowPosition());
			RenderScene(true);
		}
	}
}

void Window::RenderMessage() const
{
	if (message->IsVisible())
	{
		SDL_Rect msg_location = { 10, 10, message->Width(), message->Height() };
		SDL_RenderCopy(renderer, message->GetTexture(), nullptr, &msg_location);
	}
	SDL_RenderPresent(renderer);
}

void Window::RenderMeshes(FragmentShader& fragment_shader)
{
	for (auto mesh : scene.meshes)
	{
		std::vector<ShadedVertex> processed;
		auto back_inserter = std::back_inserter(processed);
		VertexShader::TransformCoords(mesh, scene.cameras[camera_number], back_inserter);
		fragment_shader.Paint(mesh, processed);
	}
	SDL_UpdateTexture(frame_buffer, nullptr, &pixel_data[0], SCREEN_WIDTH * 4);
	SDL_RenderCopy(renderer, frame_buffer, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void Window::RenderScene(bool click_map)
{
	memset(&pixel_data[0], 0x10, pixel_data.size());

	LightingModel* lighting_model = GetLightingModel(scene.lights, scene.cameras[camera_number]);
	FragmentShader* fragment_shader = GetShader(SCREEN_WIDTH, SCREEN_HEIGHT, pixel_data, *lighting_model);
	RenderMeshes(*fragment_shader);
	RenderMessage();
	if (click_map)
	{
		RenderClickMap();
	}
	delete lighting_model;
	delete fragment_shader;
}

void Window::RenderClickMap()
{
	memset(&click_map[0], 0xff, click_map.size());
	ClickMapFragmentShader fragment_shader(SCREEN_WIDTH, SCREEN_HEIGHT, click_map);
	for (auto mesh : scene.meshes)
	{
		std::vector<ShadedVertex> processed;
		auto back_inserter = std::back_inserter(processed);
		VertexShader::TransformCoords(mesh, scene.cameras[camera_number], back_inserter);
		fragment_shader.Paint(mesh, processed);
	}
}

Window::~Window()
{
	if (message != nullptr)
	{
		delete message;
	}
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
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

void Window::PrepareCameras() const
{
	scene.cameras.clear();
	// static
	scene.cameras.push_back(Camera(Eigen::Vector3f(16, 26, 15), Eigen::Vector3f(6, 7, 0)));
	// follow
	scene.cameras.push_back(Camera(Eigen::Vector3f(7, -6, 15), Eigen::Vector3f(7, 7, 0)));
	// object
	scene.cameras.push_back(Camera(Eigen::Vector3f(16, 26, 15), Eigen::Vector3f(6, 7, 0)));
}

void Window::FollowTarget(Eigen::Vector3f follow_position) const
{
	scene.cameras[FOLLOW_CAMERA].LookAt(follow_position);
	Eigen::Vector3f camera_pos = follow_position + Eigen::Vector3f(-15, 0, 10);
	Eigen::Vector3f look_pos = follow_position + Eigen::Vector3f(0, 0, 2);
	scene.cameras[OBJECT_CAMERA].Move(camera_pos, look_pos);
}

FragmentShader* Window::GetShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model) const
{
	switch (shader_type)
	{
	case S_FLAT:
		return new FlatFragmentShader(width, height, pixel_data, lighting_model);
	case S_GOURAUD:
		return new GouraudFragmentShader(width, height, pixel_data, lighting_model);
	case S_PHONG:
		return new PhongFragmentShader(width, height, pixel_data, lighting_model);
	}
	return nullptr;
}

LightingModel* Window::GetLightingModel(const std::vector<Light>& lights, const Camera& camera) const
{
	switch (lighting_model)
	{
	case L_BLINN_PHONG:
		return new BlinnPhongLightingModel(lights, camera); // action
	case L_PHONG:
		return new PhongLightingModel(lights, camera);
	}
	return nullptr;
}

void Window::MouseButtonUp(SDL_MouseButtonEvent e)
{
	auto offset = e.y * SCREEN_WIDTH + e.x;
	auto index = click_map[offset];
	if (index < 0 || index > scene.meshes.size()) return;
	auto message = state_manager.MeshClicked(scene.meshes[index]);
	if (message.size() > 0)
	{
		this->message->SetText(message);
		RenderScene(false);
	} 
	else
	{
		this->message->Hide();
		RenderScene(true);
	}
}

void Window::KeyDown(SDL_KeyboardEvent e)
{
	std::set<SDL_Keycode> supported_codes = 
	{
		SDL_SCANCODE_1,
		SDL_SCANCODE_2,
		SDL_SCANCODE_3,
		SDL_SCANCODE_Q,
		SDL_SCANCODE_W,
		SDL_SCANCODE_E,
		SDL_SCANCODE_R,
		SDL_SCANCODE_T
	};
	if (supported_codes.find(e.keysym.scancode) == supported_codes.end()) return;
	switch (e.keysym.scancode)
	{
	case SDL_SCANCODE_1:
		camera_number = STATIC_CAMERA;
		message->SetText("Static camera");
		break;
	case SDL_SCANCODE_2:
		camera_number = FOLLOW_CAMERA;
		message->SetText("Follow camera");
		break;
	case SDL_SCANCODE_3:
		camera_number = OBJECT_CAMERA;
		message->SetText("Object camera");
		break;
	case SDL_SCANCODE_Q:
		lighting_model = L_BLINN_PHONG;
		message->SetText("Blinn-Phong lighting model");
		break;
	case SDL_SCANCODE_W:
		lighting_model = L_PHONG;
		message->SetText("Phong lighting model");
		break;
	case SDL_SCANCODE_E:
		shader_type = S_FLAT;
		message->SetText("Flat shading");
		break;
	case SDL_SCANCODE_R:
		shader_type = S_GOURAUD;
		message->SetText("Gouraud shading");
		break;
	case SDL_SCANCODE_T:
		shader_type = S_PHONG;
		message->SetText("Phong shading");
		break;
	default:
		break;
	}
	RenderScene(true);
}
