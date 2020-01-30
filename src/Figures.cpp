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

int PDC_mbtowc( wchar_t* pwc, const char* s, size_t n ) {
	wchar_t key;
	int i = -1;
	const unsigned char* string;

	if ( !s || ( n < 1 ) )
		return -1;

	if ( !*s )
		return 0;

	string = (const unsigned char*) s;

	key = string[0];

	/* Simplistic UTF-8 decoder -- only does the BMP, minimal validation */

	if ( key & 0x80 ) {
		if ( ( key & 0xe0 ) == 0xc0 ) {
			if ( 1 < n ) {
				key = ( ( key & 0x1f ) << 6 ) | ( string[1] & 0x3f );
				i = 2;
			}
		} else if ( ( key & 0xe0 ) == 0xe0 ) {
			if ( 2 < n ) {
				key = ( ( key & 0x0f ) << 12 ) | ( ( string[1] & 0x3f ) << 6 ) |
					( string[2] & 0x3f );
				i = 3;
			}
		}
	} else
		i = 1;

	if ( i )
		*pwc = key;

	return i;
}

void Figure::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	int height = size[1];
	std::vector<PrintArea> prints{};
	for ( const auto& power : m_powerups ) {
		prints.emplace_back(PrintIterator::fromPrint(power->getPrint(size)));
	}
	chtype input[28];
	wchar_t symboles[28];
	prints.emplace_back( PrintIterator::fromPrint( this->getPrint( size ) ) );
	for ( const auto& area : prints ) {
		for ( auto itr = area.first; itr != area.second; ++itr ) {
			if ( itr->size() > 0 ) {
				int len = 0;
				{
					int off = 0;
					wchar_t* c = symboles;
					do {
						off += PDC_mbtowc( c++, reinterpret_cast<const char*>( itr->data() ) + off, itr->size() - off );
						++len;
					} while ( off != itr->size());

				}
				int xOff = ( size[0] - len ) / 2;
				wmove( wnd, pos[1] + size[1] - itr.level(), pos[0] + xOff );
				winchnstr( wnd, input, len );
				int off = 0;
				for ( int i = 0; input[i] != 0; ++i) {
					wchar_t c = 0;
					chtype atr = (input[i] & A_ATTRIBUTES) >> PDC_COLOR_SHIFT;
					short fg, bg;
					pair_content( static_cast<short>(atr), &fg, &bg );
					atr = Glob::instance().GetColorAttrib( getFractionColor(getFraction()), bg );
					input[i] = atr | ( symboles[i] & A_CHARTEXT );
				}
				waddchstr( wnd, input );
			}
		}
	}
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
Pos Figure::printInfo( WINDOW* wnd, const Pos& tl, const Pos& br ) const {
	Pos pos = tl;
	wmove( wnd, pos[1], pos[0] );
	wprintw( wnd, "Figure: %s", getName());
	pos += Pos(2, 1);
	{
		std::string_view dis = description();
		if ( dis.length() > 0 ) {
			wmove( wnd, pos[1], pos[0] );
			waddstr( wnd, dis.data() );
			pos += Pos( 0, 1 );
		}
	}
	pos += Pos( 0, 1 );
	wmove( wnd, pos[1], pos[0] );
	wprintw( wnd, "Fraction: %s", getFractionName(getFraction()).data());
	pos += Pos( 0, 2 );
	wmove( wnd, pos[1], pos[0] );
	waddstr( wnd, "Powers: " );
	pos += Pos( 2, 2 );
	for ( const auto& p : m_powerups ) {
		pos = p->printInfo( wnd, pos, br );
	}
	return Pos( tl[0], pos[1] );
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
	setMovments( getTile(), m_targets );
	return m_targets;
}

void Pawn::setMovments( const Tile_p& tile, std::vector<Tile_w>& moves ) const {
	if ( !m_queen ) {
		{
			const Tile_p& next = tile->getNeighbor( Directions.up );
			STEPS step = next->tryMove( *this );
			if ( isSet( step, STEPS::YES ) && !isSet( step, STEPS::BLOCKING ) ) {
				moves.push_back( next );
			}
		}
		{
			const Tile_p& next = tile->getNeighbor( Directions.up + Directions.left );
			STEPS step = next->tryMove( *this );
			if ( isSet( step, STEPS::BLOCKING ) ) {
				moves.push_back( next );
			}
		}
		{
			const Tile_p& next = tile->getNeighbor( Directions.up + Directions.right );
			STEPS step = next->tryMove( *this );
			if ( isSet( step, STEPS::BLOCKING ) ) {
				moves.push_back( next );
			}
		}
	} else {
		for ( const auto& dir : Directions ) {
			const Tile_p* next = &tile->getNeighbor( dir );
			if ( !( *next ) ) continue;
			STEPS step = ( *next )->tryMove( *this );
			while ( isSet( step, STEPS::YES ) ) {
				moves.push_back( *next );
				if ( isSet( step, STEPS::BLOCKING ) ) break;
				next = &( ( *next )->getNeighbor( dir ) );
				if ( !( *next ) ) break;
				step = ( *next )->tryMove( *this );
			}
		}
	}
}

std::u8string_view Pawn::getPrint( const Pos& size ) const {
	if ( size[1] > 5 ) {
		static const char8_t p[] =
		 	 u8"()\0"
			 u8")(\0"
			u8"/__\\\0"
			u8"";
		static const char8_t q[] =
			u8"\\/\0"
			u8")(\0"
			u8")(\0"
			u8"/__\\\0";
		if ( m_queen ) {
			return{ q, sizeof( q ) };
		}
		return { p, sizeof( p ) };
	}
	if ( size[1] > 2 ) {
		static const char8_t p[] =
			u8"\u2659\0"
			u8"";
		static const char8_t q[] =
			u8"\u2655\0"
			u8"";
		if ( m_queen ) {
			return { q, sizeof( q ) };
		}
		return { p, sizeof( p ) };
	}
	if ( m_queen ) {
		return u8"\u2655";
	}
	return u8"\u2659";
}

void Pawn::onTileChange( const Tile_w& oldTile, const Tile_p& newTile ) {
	if ( newTile && newTile->isPromotionZone() ) {
		m_queen = true;
	}
}

void Bishop::setMovments( const Tile_p& tile, std::vector<Tile_w>& moveList ) const {
	static constexpr std::array<Pos, 4> dirs = {
		Directions.up + Directions.left,
		Directions.up + Directions.right,
		Directions.down + Directions.left,
		Directions.down + Directions.right
	};
	for ( const auto& dir : dirs ) {
		const Tile_p* next = &tile->getNeighbor( dir );
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

std::u8string_view Bishop::getPrint( const Pos& size ) const {
	if ( size[1] > 5 ) {
		static const char8_t p[] =
		 	 u8"()\0"
			 u8")(\0"
			 u8")(\0"
			u8"/__\\\0"
			u8"";
		return { p, sizeof( p ) };

	}
	if ( size[1] > 2 ) {
		static const char8_t p[] =
			u8"\u2657\0"
			u8"";
		return { p, sizeof( p ) };
	}
	return u8"\u2657";
}

void King::setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const {
	static constexpr std::array<Pos, 8> dirs = {
		Directions.up,
		Directions.up + Directions.left,
		Directions.left,
		Directions.left + Directions.down,
		Directions.down,
		Directions.down + Directions.right,
		Directions.right,
		Directions.right + Directions.up
	};
	for ( const auto& dir : dirs ) {
		const Tile_p* next = &tile->getNeighbor( dir );
		if ( !( *next ) ) continue;
		if ( isSet( ( *next )->tryMove( *this ), STEPS::YES ) ) {
			movList.push_back( *next );
		}
	}
}

std::u8string_view King::getPrint( const Pos& size ) const {
	if ( size[1] > 5 ) {
		static const char8_t p[] =
			u8"(++)\0"
		   u8"\\  /\0"
			 u8")(\0"
			u8"/__\\\0"
			u8"";
		return { p, sizeof( p ) };

	}
	if ( size[1] > 2 ) {
		static const char8_t p[] =
			u8"\u2654\0"
			u8"";
		return { p, sizeof( p ) };
	}
	return u8"\u2654";
}

Tile_w King::getMove() { return Tile_w{}; }
Tile_w Pawn::getMove() { 
	const Tile_p& tile = getTile();
	if ( tile ) {
		std::vector<Tile_w> moves;
		setMovments( tile, moves );
		if ( moves.size() > 0 ) {
			return moves.back();
		}
	}
	return Tile_w{};
}
Tile_w Figure::extraxtGreedeMove(const std::vector<Tile_w>& moves) {
	using Target_t = std::tuple<Obj_p, Tile_p, int>;
	std::vector<Target_t> targets;
	std::vector<int> m1to2{0};
	std::vector<Tile_w> moves2;
	for ( const Tile_w& wT : moves ) {
		Tile_p pT = wT.lock();
		if ( const auto& obj = pT->getObjet() ) {
			targets.push_back( { obj, pT, 1 } );
		}
		setMovments( pT, moves2 );
		m1to2.push_back( moves2.size() );
	}
	class M{
	public:
		M( const std::vector<Tile_w>& o, const std::vector<int>& n ) 
			: out{o.begin()}, itr { n.begin() + 1}, end{ n.end() } {

		}
		Tile_p operator*() const { return out->lock(); }
		operator bool() const { return itr != end; }
		M& operator++() {
			if ( itr == end ) { throw std::string("out of limit!"); }
			if ( ++count == *itr ) {
				++itr;
				++out;
			}
			return *this;
		}
	private:
		int count{ 0 };
		std::vector<Tile_w>::const_iterator out;
		std::vector<int>::const_iterator itr, end;
	} mv1(moves, m1to2);
	for ( const Tile_w& wT : moves2 ) {
		Tile_p pT = wT.lock();
		if ( const auto& obj = pT->getObjet() ) {
			targets.push_back( { obj, *mv1, 2 } );
		}
		++mv1;
	}
	if ( targets.empty() ) { return moves.front(); }
	struct {
		bool operator()( const Target_t& l_h, const Target_t& r_h ) const {
			auto pl = std::get<Tile_p>( l_h ).get(), pr = std::get<Tile_p>( r_h ).get();
			if ( pl != pr ) { return pl > pr; }
			auto ml = std::get<int>( l_h ), mr = std::get<int>( r_h );
			if ( ml != mr ) { return ml > mr; }
			auto tl = std::get<Obj_p>( l_h )->getObjectType(), tr = std::get<Obj_p>( r_h )->getObjectType();
			return static_cast<int>( tl ) > static_cast<int>( tr );
		}
	} mySort;
	std::sort( targets.begin(), targets.end(), mySort );
	std::vector<std::array<decltype( targets )::iterator, 2>> batches{};
	{
		auto b = targets.begin();
		for ( auto itr = targets.begin(); itr != targets.end(); ++itr ) {
			if ( std::get<Tile_p>( *itr ) != std::get<Tile_p>( *b ) ) {
				batches.push_back( {b, itr} );
				b = itr;
			}
		}
		batches.push_back( {b, targets.end()} );
	}
	for ( const auto& batch : batches ) { // move 1 and move 2 access
		if ( std::get<int>(*(batch[0])) == 1 && std::get<int>(*(batch[0]))) {
			return std::get<Tile_p>(*(batch[0]));
		}
	}
	for ( const auto& batch : batches ) { // move 1 
		if ( std::get<int>(*(batch[0])) == 1) {
			return std::get<Tile_p>(*(batch[0]));
		}
	}
	return std::get<Tile_p>(targets.front());
}

Tile_w Bishop::getMove() { 
	const Tile_p& tile = getTile();
	if ( tile ) {
		std::vector<Tile_w> moves;
		setMovments( tile, moves );
		if ( moves.size() > 0 ) {
			return extraxtGreedeMove( moves );
		}
	}
	return Tile_w{};
}

Tile_w Knight::getMove() { 
	if ( const Tile_p& tile = getTile() ) {
		std::vector<Tile_w> moves;
		setMovments( tile, moves );
		if ( moves.size() > 0 ) {
			return extraxtGreedeMove( moves );
		}
	}
	return Tile_w{}; 
}

void Knight::setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const {
	constexpr std::array<Pos, 4> dirs = { Directions.up, Directions.left, Directions.down, Directions.right };
	for ( const Pos& dir : dirs ) {
		const Tile_p& up = tile->getNeighbor( dir );
		if ( up ) {
			const Tile_p& uup = up->getNeighbor( dir );
			if ( uup ) {
				const Pos left( dir[1], dir[0] );
				const Tile_p& uupl = uup->getNeighbor( left );
				if ( uupl && isSet( uupl->tryMove( *this ), STEPS::YES ) ) { movList.push_back( uupl ); }
				const Pos right = left * -1 ;
				const Tile_p& uupr = uup->getNeighbor( right );
				if ( uupr && isSet( uupr->tryMove( *this ), STEPS::YES ) ) { movList.push_back( uupr ); }
			}
		}
	}
}

std::u8string_view Knight::getPrint( const Pos& size ) const {
	if ( size[1] > 5 ) {
		static const char8_t p[] =
			u8"_''\0"
			u8"\"-=\\~\0"
			u8")(\0"
			u8"/__\\\0"
			u8"";
		return { p, sizeof( p ) };

	}
	if ( size[1] > 2 ) {
		static const char8_t p[] =
			u8"\u2658\0"
			u8"";
		return { p, sizeof( p ) };
	}
	return u8"\u2658";
}

