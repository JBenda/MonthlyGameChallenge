#include "Board.hpp"
#include <functional>
#include <algorithm>

void Object::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
}


Tile::Tile() : m_self(Tile_p(this)){ }

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
	if(!m_objs.insert(obj).second) {
		throw std::string("layer collision!");
	}
	obj->setTile(m_self);
}

const Tile_p* Board::getTileEx(const Pos& pos) {
	auto itr = m_map.find(pos);
	if (itr == m_map.end()) { return nullptr; }
	return &(itr->second);
}



Board::Board(const Pos& pos) : m_tileSize(pos){}

Tile& Board::getTile(const Pos& pos) {
	const Tile_p* ptr = getTileEx(pos);
	if(!ptr) throw std::string("Try to access not existing Tile: ") + pos.str();
	return *(*ptr);
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

bool Object::less::operator()(const Obj_p& l_h, const Obj_p& r_h) {
	return static_cast<int>(l_h->m_layer ) 
		< static_cast<int>(r_h->m_layer);
}

void Tile::removeObject(const Obj_p& obj) {
	for(auto itr = m_objs.begin(); itr != m_objs.end(); ++itr) {
		if (*itr == obj) {
			m_objs.erase(itr);
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
		tile->removeObject(obj);
		target->addObject(obj);		
	}
}

void Board::moveOrOut(const Obj_p& obj, const Pos& dir) {
	const Tile_p& tile = obj->getTile();
	tryMove(obj, dir);
	if (obj->getTile() == tile) {
		tile->removeObject(obj);
	}
}
