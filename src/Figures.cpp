#include "Figures.hpp"
#include "PowerUp.hpp"

bool Figure::onCollision( const Obj_p& obj ) {
	bool alive = true;
	for ( const auto& power : m_powerups ) {
		alive = power->onCollision( obj ) ? alive : false;
	}
	if ( !alive ) {
		return false;
	}
	if ( obj->getObjectType() == OBJECT::Figure ) {
		const Figure& figure = static_cast<const Figure&>( *obj );
		if ( static_cast<const Figure&>( *this ).getFraction() != figure.getFraction() ) {
			getTile()->removeObject( *this );
		}
	}
	return true;
}

void Figure::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	wmove( wnd, pos[1], pos[0] );
	int format = winch( wnd ) & A_COLOR;
	wattron( wnd, format );
	int height = size[1];
	const auto area = this->getPrint( height );
	for ( auto itr = area.first; itr != area.second; ++itr ) {
		int xOff = (size[0] - itr->size()) / 2;
		wmove( wnd, pos[1] + size[1] - itr.level(), pos[0] + xOff);
		waddstr( wnd, reinterpret_cast<const char*>(itr->data()) );
	}

	wattroff( wnd, format );
}

void Figure::addPowerUp( const Power_p& power ) {
	m_powerups.emplace_back(power);
}

void Figure::removePowerUp( const Power_p& power ) {
	for ( auto itr = m_powerups.begin(); itr != m_powerups.end(); ++itr ) {
		if ( *itr == power ) {
			m_powerups.erase(itr);
			return;
		}
	}
}

Figure::PrintArea Figure::PrintIterator::fromPrint( const std::u8string_view& print ) {
	return std::make_pair(PrintIterator(print), PrintIterator());
}

Figure::PrintIterator& Figure::PrintIterator::operator++() {
	if ( m_msg.size() == 0 ) {
		m_lvl = -1;
		return *this;
	}
	std::size_t p = m_msg.find_last_of( u8'\0' );
	if ( p == std::u8string_view::npos ) {
		m_now = m_msg;
		m_msg.remove_suffix( m_now.size() );
	} else { 
		m_now = m_msg.substr( p + 1 ); 
	m_msg.remove_suffix( m_now.size() + 1);
	}
	++m_lvl;
	return *this;
}

const std::vector<Tile_w>& Figure::getMovments() {
	m_targets.resize( 0 );
	setMovments( m_targets );
	return m_targets;
}

void Pawn::setMovments( std::vector<Tile_w>& moves ) const {
	{
		const Tile_p& next = getTile()->getNeighbor( Directions.up );
		STEPS step = next->tryMove( *this );
		if ( isSet( step, STEPS::YES ) && !isSet( step, STEPS::BLOCKING ) ) {
			moves.push_back( next );
		}
	} {
		const Tile_p& next = getTile()->getNeighbor( Directions.up + Directions.left );
		STEPS step = next->tryMove( *this );
		if ( isSet( step, STEPS::BLOCKING ) ) {
			moves.push_back( next );
		}
	} {
		const Tile_p& next = getTile()->getNeighbor( Directions.up + Directions.right );
		STEPS step = next->tryMove( *this );
		if ( isSet( step, STEPS::BLOCKING ) ) {
			moves.push_back( next );
		}
	}
}

Figure::PrintArea Pawn::getPrint( const int height ) {
	if ( height > 5 ) {
		static const char8_t p[] =
		 	 u8"()\0"
			 u8")(\0"
			u8"/__\\\0"
			u8"";
		return PrintIterator::fromPrint( { p, sizeof( p ) } );

	}
	return PrintIterator::fromPrint(u8"\u2659");
}

void Bishop::setMovments( std::vector<Tile_w>& moveList ) const {
	static constexpr std::array<Pos, 4> dirs = {
		Directions.up + Directions.left,
		Directions.up + Directions.right,
		Directions.down + Directions.left,
		Directions.down + Directions.right
	};
	for ( const auto& dir : dirs ) {
		const Tile_p* next = &getTile()->getNeighbor( dir );
		if ( !(*next) ) continue;
		STEPS step = (*next)->tryMove( *this );
		while ( isSet( step, STEPS::YES ) ) { 
			moveList.push_back( *next );
			if ( isSet( step, STEPS::BLOCKING ) ) break;
			next = &((*next)->getNeighbor( dir ));
			if ( !(*next) ) break;
			step = ( *next )->tryMove( *this );
		}
	}
}

Figure::PrintArea Bishop::getPrint( const int height ) {
	if ( height > 5 ) {
		static const char8_t p[] =
		 	 u8"()\0"
			 u8")(\0"
			 u8")(\0"
			u8"/__\\\0"
			u8"";
		return PrintIterator::fromPrint( { p, sizeof( p ) } );

	}
	return PrintIterator::fromPrint(u8"\u2659");
}

