#include "Animator.hpp"

void Animator::addAnimation( Animator::Animation&& animation ) {
	m_animations.emplace_back( animation );
}

void Animator::update( const std::chrono::duration<float>& dt ) {
	std::vector<typename decltype( m_animations )::iterator> m_remove;
	for ( auto itr = m_animations.begin(); itr != m_animations.end() && !itr->isBarrier(); ++itr ) {
		itr->update( dt );
	}
	while ( !m_animations.empty() && m_animations.front().isFinished() ) {
		m_animations.front().clear();
		m_animations.pop_front();
	}
}

void Animator::draw( WINDOW* wnd ) const {
	for ( const auto& animation : m_animations ) {
		animation.draw( wnd );
	}
}

void Animator::Animation::update( const std::chrono::duration<float>& dt ) {
	m_time -= dt;
}

void Animator::Animation::draw( WINDOW* wnd ) const {
	if ( isBarrier() ) { return; }
	float t = 1.f - std::max( m_time.count(), 0.f ) / m_tStart;
	m_obj->draw( wnd,
				 std::lerp( m_beginP, m_endP, t ),
				 std::lerp( m_beginS, m_endS, t ) );
}
