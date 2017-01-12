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

enum LightingModelType
{
	L_PHONG = 0,
	L_BLINN_PHONG = 1
};

enum FragmentShaderType
{
	S_FLAT = 0,
	S_GOURAUD = 1,
	S_PHONG = 2
};

class Window
{
public:
	Window(const std::string& _title, Scene& scnee);
	Window(const Window& _other) = delete;
	Window& operator=(const Window& _other) = delete;
	void Show();
	void RenderMessage() const;
	void RenderMeshes(FragmentShader& fragment_shader);
	void RenderScene(bool click_map);
	void RenderClickMap();
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
	CameraNumber camera_number = STATIC_CAMERA;
	LightingModelType lighting_model = L_PHONG;
	FragmentShaderType shader_type = S_FLAT;

	std::vector<unsigned char> pixel_data;
	std::vector<unsigned char> click_map;
	
	int ScreenX(const ShadedVertex& coords) const;
	int ScreenY(const ShadedVertex& coords) const;
	static bool InBound(const int coord, const int max);

	void PrepareCameras() const;
	void FollowTarget(Eigen::Vector3f follow_position) const;
	FragmentShader* GetShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model) const;
	LightingModel* GetLightingModel(const std::vector<Light>& lights, const Camera& camera) const;

	// events
	void MouseButtonUp(SDL_MouseButtonEvent e);
	void KeyDown(SDL_KeyboardEvent e);
};