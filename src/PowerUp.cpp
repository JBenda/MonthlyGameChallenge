#include "PowerUp.hpp"
#include "Figures.hpp"

bool Konter::onCollision( const Obj_p& obj ) {
	if ( obj->getObjectType() == OBJECT::Figure ) {
		if ( m_figure.expired() ) {
			std::shared_ptr<Figure> figure = std::static_pointer_cast<Figure>( obj );
			m_figure = figure;
			figure->addPowerUp( self() );
			getTile()->removeObject( *this );
			return true;
		} else {
			m_figure.lock()->removePowerUp( self() );
			return false;
		}
	}
	return true;
}

std::u8string_view Konter::getPrint( const Pos& size ) {
	if ( size[1] >= 7 && size[0] >= 14 ) {
		static const char8_t p[] =
			u8"   x      x   \0"
			u8"  x        x  \0"
			u8" x          x \0"
			u8" x          x \0"
			u8" x          x \0"
			u8"  x        x  \0"
			u8"   x      x   ";
		return { p, sizeof( p ) };
	}
	if ( size[1] >= 3 && size[0] >= 3 ) {
		static const char8_t p[] =
			u8"( )\0";
		return { p, sizeof( p ) };
	}
	return u8"";
}

Pos PowerUp::printInfo( WINDOW* wnd, const Pos& tl, const Pos& br ) const {
	const std::string_view description = getDescription();
	if ( !getTile() ) {
		wmove( wnd, tl[1], tl[0] );
		waddstr( wnd,  description.data());
		return tl + Pos( 0, 1 );
	} else {
		wmove( wnd, tl[1], tl[0] );
		wprintw( wnd, "PowerUp: %s", getName().data() );
		wmove( wnd, tl[1] + 1, tl[0] + 2 );
		waddstr( wnd, getDescription().data() );
		return tl + Pos( 0, 2 );
	}
}

