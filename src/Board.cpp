#include "Board.hpp"
#include "Figures.hpp"
#include <functional>
#include <algorithm>

void Object::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
}

void Tile::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	for( const Obj_p& obj : m_objs) {
		obj->draw(wnd, pos, size);
	}
}

void Tile::link(
		const Pos& dir, const Tile_p& from, const Tile_p& to) {
	from->m_neighbors[Directions.toId(dir)] = to;
	to->m_neighbors[Directions.mirrow(Directions.toId(dir))] = from;
}

void Tile::addObject(const Obj_p& obj) {
	auto itr = m_objs.find( obj );
	bool insert = true;
	if ( itr != m_objs.end() ) {
		insert = ( *itr )->onCollision( obj );
	}
	if( insert ) {
		if ( !m_objs.insert( obj ).second ) {
			throw std::string( "layer collision!" );
		}
		obj->setTile( shared_from_this() );
	} else {
		obj->setTile( nullptr );
	}
}

const Tile_p* Board::getTileEx(const Pos& pos) {
	auto itr = m_map.find(pos);
	if (itr == m_map.end()) { return nullptr; }
	return &(itr->second);
}

STEPS Tile::tryMove( const Object& obj ) const {
	Obj_w objOnTile = getObjet();
	if ( objOnTile.expired() ) return STEPS::YES;
	const Object& objOnT = *( objOnTile.lock() );
	if ( obj.getObjectType() == OBJECT::Figure
		 && objOnT.getObjectType() == OBJECT::Figure) {
		if ( static_cast<const Figure&>( obj ).getFraction() != static_cast<const Figure&>( objOnT ).getFraction() ) {
			return STEPS::BLOCKING;
		}
	}
	return STEPS::NO;
}

Board::Board(const Pos& pos) : m_tileSize(pos){}

const Tile_p& Board::getTile(const Pos& pos) {
	const Tile_p* ptr = getTileEx(pos);
	if(!ptr) throw std::string("Try to access not existing Tile: ") + pos.str();
	return *ptr;
}

void Board::draw(WINDOW* wnd) {
	for(const auto& itr : m_map) {
		itr.second->draw(
			wnd,
			{itr.first[0] * m_tileSize[0], itr.first[1] * m_tileSize[1]},
			m_tileSize);
	}
}

void Board::setTile(const Pos& pos, const Tile_p& tile) {
	m_map[pos] = tile;
	for(const Pos& d : Directions) {
		Pos p = pos + d;
		auto neighbor = getTileEx(p);
		if(neighbor) {
			Tile::link(d, tile, *neighbor); 
		}	
	}
}


 const Tile_p& Tile::getNeighbor( const Pos& dir ) {
	 return m_neighbors[Directions.toId( dir )];
}

bool Object::less::operator()(const Obj_p& l_h, const Obj_p& r_h) const {
	return static_cast<int>(l_h->m_layer ) 
		< static_cast<int>(r_h->m_layer);
}

void Tile::removeObject( const Object& obj ) {
	for ( auto itr = m_objs.begin(); itr != m_objs.end(); ++itr ) {
		if ( itr->get() == &obj ) {
			m_objs.erase( itr );
			return;
		}
	}
	throw std::string("can't remove Object, it don't exist at this place");
}

void Board::tryMove(const Obj_p& obj, const Pos& dir) {
	const Tile_p& tile = obj->getTile();
	if(!tile) throw std::string("object has no Tile! But should move?");
	const Tile_p&  target = tile->getNeighbor(dir);
	if(target) {
		tile->removeObject(*obj);
		target->addObject(obj);		
	}
}

void Board::move( const Obj_p& obj, Tile& tile ) {
	const Tile_p& s_tile = obj->getTile();
	if ( !s_tile ) throw std::string("obj has no tiel");
	s_tile->removeObject( *obj );
	tile.addObject( obj );
}

void Board::moveOrOut(const Obj_p& obj, const Pos& dir) {
	const Tile_p& tile = obj->getTile();
	tryMove(obj, dir);
	if (obj->getTile() == tile) {
		tile->removeObject(*obj);
	}
}

const Obj_p& Tile::getLayer( const LAYER layer ) const {
	for ( const Obj_p& obj : m_objs ) {
		if ( obj->getLayer() == layer ) {
			return obj;
		}
	}
	static const Obj_p obj(nullptr);
	return obj;
}
