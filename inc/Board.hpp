#pragma once

#include <unordered_map>
#include <array>
#include <memory>
#include <vector>
#include <string>
#include <optional>

#include "config.hpp"

class Tile;
class Object {
public:
	virtual void draw(WINDOW* wnd, const Pos& pos, const Pos& size);
protected:
	Object() = default;
	Object( LAYER layer, OBJECT objT ) : m_layer{ layer }, m_objT{ objT }, m_power{0}{}
private:
	LAYER m_layer;
	OBJECT m_objT;
	std::size_t m_power{0};
};
using Obj_p = std::shared_ptr<Object>;

class Tile;
using Tile_p = std::shared_ptr<Tile>;

class Node {
public:
	Node() = default;
	Node(const Obj_p& obj) : m_obj{obj}{}
	const Tile& getTile() const {
		if(m_tile) return *m_tile;
		return m_next->getTile();
	}
	template<typename FN_t, typename ... Args>
	void forEach(FN_t fn, Args ... args) {
			if (m_obj) {
				((*m_obj).*fn)(std::forward<Args>(args)...);
			}
			if(m_next) {
				m_next->forEach(std::forward<FN_t>(fn), 
						std::forward<Args>(args)...);
			}	
	}
	void add(const Obj_p& obj) {
		if(m_next) { 
			m_next->add(obj);
		} else {
			m_next = std::make_unique<Node>(obj);
		}
	}
protected:
	Node(Tile& tile) : m_tile(&tile) {}
private:
	std::shared_ptr<Object> m_obj {nullptr};
	std::unique_ptr<Node> m_next {nullptr};
	Tile* m_tile {nullptr};
};



class Board {
	using Map_t = std::unordered_map<Pos, std::shared_ptr<Tile>>;
	void rewrite(const Pos& pos, const Tile_p& tile);
	/*
	 * @brief return Tile pointer or nullptr 
	 */
	const Tile_p* getTileEx(const Pos& pos);
public:
	Board(const Pos& tileSize);
	void setTileSize(const Pos& size) { m_tileSize = size; }
	const Pos& getTileSize() { return m_tileSize; }
	void setTile(const Pos& pos, const Tile_p& tile);
	Tile& getTile(const Pos& pos);
	void setObject(const Pos& pos, const Obj_p& obj);
	void draw(WINDOW* wnd);
private:
	Map_t m_map;
	Pos m_tileSize;
};

class Tile {
	class BaseNode : public Node {
	public:
		BaseNode(Tile& tile) : Node(tile){ }
	};
public:
	Tile();
	~Tile() {}
	void draw(WINDOW* wnd, const Pos& pos, const Pos& size); 
	static void link(
			const Pos& dir, const Tile_p& from, const Tile_p& to 
		);
	void addObject(const Obj_p& obj) {
		m_root->add(obj);	
	}
	Tile_p getNeighbor( const Pos& dir);
private:
	virtual void drawSelf(WINDOW* wnd, const Pos& pos, const Pos& size){}
	std::unique_ptr<BaseNode> m_root = nullptr;
	std::array<Tile_p, 9> m_neighbors = {
		nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr
	};
};
