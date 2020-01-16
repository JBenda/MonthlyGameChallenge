#include "Board.hpp"
#include <functional>

void Object::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
}


Tile::Tile() : m_root(std::make_unique<BaseNode>(*this)){ }

void Tile::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	drawSelf(wnd, pos, size);
	m_root->forEach(&Object::draw, wnd, pos, size);
}

void Tile::link(
		const Pos& dir, const Tile_p& from, const Tile_p& to) {
	from->m_neighbors[Directions.toId(dir)] = to;
	to->m_neighbors[Directions.mirrow(Directions.toId(dir))] = from;
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

void Board::setObject(const Pos& pos, const Obj_p& obj) {
	getTile(pos).addObject(obj);	
}

 Tile_p Tile::getNeighbor( const Pos& dir ) {
	 m_neighbors[NeighborsC::toId( dir )];
}
