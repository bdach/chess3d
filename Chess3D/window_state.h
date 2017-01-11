#pragma once
#include "mesh.h"

class State
{
public:
	virtual ~State() = default;
	virtual std::string MeshClicked(Mesh& mesh) { return std::string(); }
	virtual bool NextFrame(int ticks, Eigen::Vector3f& position) { return false; }

	bool IsDone() const { return done; }
	State* NextState() const { return next_state; }
protected:
	bool done = false;;
	State* next_state = nullptr;
};

class StateManager
{
public:
	StateManager();
	~StateManager();

	void AdvanceStates();
	std::string MeshClicked(Mesh& mesh) const;
	bool NextFrame();
	Eigen::Vector3f FollowPosition();
private:
	State* state;
	unsigned int last_ticks;
	Eigen::Vector3f position;
};

class IdleState : public State
{
protected:
	std::string MeshClicked(Mesh& mesh) override;
};

class PieceSelectedState : public State
{
public:
	explicit PieceSelectedState(Mesh& piece);
protected:
	std::string MeshClicked(Mesh& mesh) override;

	Mesh& piece;
};

class AnimationState : public State
{
public:
	AnimationState(Mesh& piece, const Mesh& field);
protected:
	bool NextFrame(int ticks, Eigen::Vector3f& position) override;

	Mesh& piece;
	Eigen::Vector3f start;
	Eigen::Vector3f end;
	const int ANIMATION_LENGTH = 2000;
	int remaining;
};