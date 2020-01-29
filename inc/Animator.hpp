#pragma once
#include "config.hpp"
#include "Board.hpp"

#include <deque>
#include <chrono>
#include <functional>

class Animator {
public:
	class Animation {
	public:
		using State = std::pair<const Pos&, const Pos&>;
		using Duration = std::chrono::duration<float>;
		Animation() {}
		Animation( std::function<void( void )> callback ) : m_finCallback{ callback } {}
		Animation( 
			const State& begin, 
			const State& end, 
			const Duration& duration, 
			const Obj_p& obj, 
			std::function<void( void )>&& callback = std::function<void( void )>{} )
			: m_beginP{ begin.first }, m_beginS{ begin.second }, m_endP{ end.first }, m_endS{ end.second },
			m_time{ duration }, m_tStart{ duration.count() }, m_obj{ obj }, m_finCallback{callback}{
			if ( m_tStart == 0 ) {
				throw std::string("try to construct inavalid Animation!");
			}
			m_obj->setInAnimation( true );
		}
		void disableSameObj( const Animation& anim ) { if ( anim.m_obj.get() == m_obj.get() ) { m_disable = true; } }
		void enable() { m_disable = false; }
		bool isBarrier() const { return m_tStart == 0; }
		bool isFinished() const { return isBarrier() || m_time <= decltype(m_time)::zero(); }
		bool isEnable() const { return !m_disable; }
		void update( const std::chrono::duration<float>& dt );
		void draw( WINDOW* wnd ) const ;
		void clear() { 
			if ( !isBarrier() ) { m_obj->setInAnimation( false ); }
			if ( m_finCallback ) { m_finCallback(); }
		}
	private:
		bool m_disable{ false };
		Pos m_beginP{ 0,0 }, m_beginS{0,0};
		Pos m_endP{ 0,0 }, m_endS{ 0,0 };
		std::chrono::duration<float> m_time{0};
		const float m_tStart{ 0 };
		std::shared_ptr<Object> m_obj{};
		std::function<void( void )> m_finCallback{};
	};
	void addAnimation( const Animation& ) ;
	void addBarrier( std::function<void( void )>&& callback = std::function<void( void )>{} ) { addAnimation( Animation( callback ) ); }
	bool inQueue() const { return !m_animations.empty(); }
	void update( const std::chrono::duration<float>& dt );
	void draw( WINDOW* wnd ) const;
private:
	std::deque<Animation> m_animations;
}; 