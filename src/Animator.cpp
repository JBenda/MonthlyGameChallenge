#include "Animator.hpp"

void Animator::addAnimation( const Animator::Animation& animation ) {
	Animation newA( animation );
	for ( auto& ani : m_animations ) {
		newA.disableSameObj( ani );
	}
	m_animations.emplace_back( animation );
}

void Animator::update( const std::chrono::duration<float>& dt ) {
	std::vector<typename decltype( m_animations )::iterator> m_remove;
	for ( auto itr = m_animations.begin(); itr != m_animations.end() && !itr->isBarrier(); ++itr ) {
		itr->update( dt );
	}
	bool rmBarrier = false;
	while ( !m_animations.empty() && m_animations.front().isFinished() ) {
		if ( m_animations.front().isBarrier() ) {
			rmBarrier = true;
		}
		m_animations.front().clear();
		m_animations.pop_front();
	}
	if ( rmBarrier ) {
		for ( auto itr = m_animations.begin(); itr != m_animations.end() && !itr->isBarrier(); ++itr ) {
			itr->enable();
		}
	}
}

void Animator::draw( WINDOW* wnd ) const {
	for ( const auto& animation : m_animations ) {
		if ( animation.isEnable() )animation.draw( wnd );
	}
}

void Animator::Animation::update( const std::chrono::duration<float>& dt ) {
	m_disable = false;
	m_time -= dt;
}

void Animator::Animation::draw( WINDOW* wnd ) const {
	if ( isBarrier() ) { return; }
	float t = 1.f - std::max( m_time.count(), 0.f ) / m_tStart;
	m_obj->draw( wnd,
				 std::lerp( m_beginP, m_endP, t ),
				 std::lerp( m_beginS, m_endS, t ) );
}
