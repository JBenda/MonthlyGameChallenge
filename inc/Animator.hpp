#pragma once
#include "config.hpp"
#include "Board.hpp"

#include <queue>
#include <chrono>

class Animator {
public:
	class Animation {
	public:
		using State = std::pair<const Pos&, const Pos&>;
		using Duration = std::chrono::duration<float>;
		Animation();
		Animation( const State& begin, const State& end, const Duration& duration, const Obj_p& obj )
			: m_beginP{ begin.first }, m_beginS{ begin.second }, m_endP{ end.first }, m_endS{ end.second },
			m_time{ duration }, m_obj{ obj }{}
	private:
		Pos m_beginP{ 0,0 }, m_beginS{0,0};
		Pos m_endP{ 0,0 }, m_endS{ 0,0 };
		std::chrono::duration<float> m_time{0};
		std::shared_ptr<Object> m_obj{};
	};
	void addAnimation( Animation&& ) {}
	void addBarrier() { addAnimation( Animation() ); }
	bool inQueue() const { return !m_animations.empty(); }
	void update( const std::chrono::duration<float>& dt ) {}
	void draw( WINDOW* wnd ) {}
private:
	std::queue<Animation> m_animations;
}; 