#pragma once

#include <unordered_map>
#include <array>
#include <memory>
#include <vector>
#include <string>
#include <optional>

#include "config.hpp"

class Tile;
using Tile_p = std::shared_ptr<Tile>;

class Object;
using Obj_p = std::shared_ptr<Object>;

class Board {
	using Map_t = std::unordered_map<Pos, std::shared_ptr<Tile>>;
	void rewrite(const Pos& pos, const Tile_p& tile);
	/*
	 * @brief return Tile pointer or nullptr 
	 */
	const Tile_p* getTileEx(const Pos& pos);
public:
	static void tryMove(const Obj_p& obj, const Pos& dir);
	static void moveOrOut(const Obj_p& obj, const Pos& dir);
	Board(const Pos& tileSize);

	void setTileSize(const Pos& size) { m_tileSize = size; }
	const Pos& getTileSize() { return m_tileSize; }

	void setTile(const Pos& pos, const Tile_p& tile);
	Tile& getTile(const Pos& pos);

	void draw(WINDOW* wnd);
private:
	Map_t m_map;
	Pos m_tileSize;
};

class Tile {
public:
	Tile();
	~Tile() {}
	const Tile_p& self() { return m_self; }
	void draw(WINDOW* wnd, const Pos& pos, const Pos& size); 
	static void link(
			const Pos& dir, const Tile_p& from, const Tile_p& to 
		);
	void addObject(const Obj_p& obj) ;
	void removeObject(const Obj_p& obj) ;
	const Tile_p& getNeighbor( const Pos& dir);
private:
	Tile_p m_self;
	std::vector<Obj_p> m_objs;
	std::array<Tile_p, 9> m_neighbors = {
		nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr
	};
};

class Object {
	friend void Tile::addObject(const Obj_p&);
	friend class Board;
	void setTile(const Tile_p& tile) {
		m_tile = tile;
	}
	Tile_p getTile() const { return m_tile.lock(); }
public:
	virtual void draw(WINDOW* wnd, const Pos& pos, const Pos& size);
protected:
	Object() = default;
	Object( LAYER layer, OBJECT objT ) : m_layer{ layer }, m_objT{ objT }, m_power{0}{}
private:
	LAYER m_layer {LAYER::None};
	OBJECT m_objT {OBJECT::None};
	std::weak_ptr<Tile> m_tile {};
	std::size_t m_power{0};
};
