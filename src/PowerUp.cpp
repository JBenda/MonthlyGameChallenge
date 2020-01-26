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
