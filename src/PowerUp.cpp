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