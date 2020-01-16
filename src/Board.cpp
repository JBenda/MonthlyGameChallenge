#include "Board.hpp"
#include <ncurses.h> 
#include <functional>

void Object::draw(const Pos& pos) {
	// mvprintw(0,0, "Hello  \uE29883 :) %d,%d", pos[0], pos[1]);
}


Tile::Tile() : m_root(std::make_unique<BaseNode>(*this)){ }

void Tile::draw(const Pos& pos) {
	drawSelf(pos);
	m_root->forEach(&Object::draw, pos);
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

const Tile_p& Board::getTile(const Pos& pos) {
	const Tile_p* ptr = getTileEx(pos);
	if(!ptr) throw std::string("Try to access not existing Tile: ") + pos.str();
	return *ptr;
}

void Board::draw() {
	for(const auto& itr : m_map) {
		itr.second->draw(itr.first);
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
	getTile(pos)->addObject(obj);	
}
