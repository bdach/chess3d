#include <SDL.h>
#include "window_state.h"
#include <iostream>

StateManager::StateManager()
{
	state = new IdleState();
	last_ticks = SDL_GetTicks();
}

StateManager::~StateManager()
{
	if (state != nullptr)
	{
		delete state;
	}
}

void StateManager::AdvanceStates()
{
	while (state->IsDone())
	{
		auto old = state;
		state = state->NextState();
		if (state == nullptr) throw std::exception("State was null");
		delete old;
	}
}

std::string StateManager::MeshClicked(Mesh& mesh) const
{
	return state->MeshClicked(mesh);
}

bool StateManager::NextFrame()
{
	auto ticks = SDL_GetTicks();
	auto result = state->NextFrame(ticks - last_ticks, position);
	last_ticks = ticks;
	return result;
}

Eigen::Vector3f StateManager::FollowPosition()
{
	return position;
}

std::string IdleState::MeshClicked(Mesh& mesh)
{
	if (mesh.mesh_type == PIECE)
	{
		next_state = new PieceSelectedState(mesh);
		done = true;
		return mesh.name;
	}
	return "Please select a piece.";
}

PieceSelectedState::PieceSelectedState(Mesh& piece) : piece(piece)
{
}

std::string PieceSelectedState::MeshClicked(Mesh& mesh)
{
	if (mesh.mesh_type == FIELD)
	{
		next_state = new AnimationState(piece, mesh);
		done = true;
		return piece.name + " to " + mesh.name;
	}
	return "Please select a field.";
}

AnimationState::AnimationState(Mesh& piece, const Mesh& field) : piece(piece)
{
	start = piece.GetPosition();
	end = field.GetPosition();
	remaining = ANIMATION_LENGTH;
}

bool AnimationState::NextFrame(int ticks, Eigen::Vector3f& position)
{
	auto toMove = ticks < remaining ? ticks : remaining;
	remaining -= toMove;
	auto t = static_cast<float>(remaining) / ANIMATION_LENGTH;
	position = end * (1.0f - t) + start * t;
	piece.Move(position);
	if (remaining == 0)
	{
		done = true;
		next_state = new IdleState();
	}
	return true;
}
