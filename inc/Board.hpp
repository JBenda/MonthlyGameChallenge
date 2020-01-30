#pragma once

#include <unordered_map>
#include <array>
#include <memory>
#include <set>
#include <string>
#include <optional>

#include "config.hpp"

constexpr std::array<Pos, 2> SupportedTieldSizes = { { {14,7}, {5,3},} };

enum class STEPS { NO = 0, YES = 0b1, BLOCKING = 0b11};
template<typename T>
bool isSet( T config, T flag ) {
	return (static_cast<unsigned int>( config ) & static_cast<unsigned int>( flag ))
		== static_cast<unsigned int>(flag);

}

class Tile;
using Tile_p = std::shared_ptr<Tile>;
using Tile_w = std::weak_ptr<Tile>;

class Object;
using Obj_p = std::shared_ptr<Object>;
using Obj_w = std::weak_ptr<Object>;

class Board {
	using Map_t = std::unordered_map<Pos, std::shared_ptr<Tile>>;
	/*
	 * @brief return Tile pointer or nullptr 
	 */
	const Tile_p* getTileEx(const Pos& pos);
public:
	static void move(const Obj_p& obj, Tile& tile );
	static void tryMove(const Obj_p& obj, const Pos& dir);
	static void moveOrOut(const Obj_p& obj, const Pos& dir);
	Board(const Pos& tileSize);

	void setTileSize(const Pos& size) { m_tileSize = size; }
	const Pos& getTileSize() { return m_tileSize; }

	void setTile(const Pos& pos, const Tile_p& tile);
	const Tile_p& getTile(const Pos& pos);

	const Pos& getPosition( const Tile& tile ) {
		for ( auto itr = m_map.begin(); itr != m_map.end(); ++itr ) { 
			if ( itr->second.get() == &tile ) {
				return itr->first;
			}
		}
		throw std::string("tile don't exist at this board");
	}

	void draw(WINDOW* wnd);
	auto begin() { return m_map.begin(); }
	auto end() { return m_map.end(); }
private:
	Map_t m_map;
	Pos m_tileSize;
};

class Object {
	friend class Tile;
	friend class Board;
	void setTile(const Tile_p& tile) {
		onTileChange( m_tile, tile );
		m_tile = tile;
	}
public:
	struct less {
		using is_transparent = std::true_type;
		bool operator() (const Obj_p& l_h, const Obj_p& r_h) const; 
		bool operator() ( const Obj_p& l_h, const LAYER& r_h ) const;
		bool operator() (const LAYER& l_h, const Obj_p& r_h ) const;
	};
	virtual void draw(WINDOW* wnd, const Pos& pos, const Pos& size);
	void drawIfNotAnimated( WINDOW* wnd, const Pos& pos, const Pos& size ) { if ( !m_inAnimation ) { draw( wnd, pos, size ); } }
	void setInAnimation( bool state );
	const Tile_p getTile() const { return std::move(m_tile.lock()); }
	const LAYER getLayer() const { return m_layer;}
	const OBJECT getObjectType() const { return m_objT; }
	/// @return false if obj shoudl be deleted
	virtual bool onCollision( const Obj_p& obj ) { throw std::string("no collision behavior defined!"); };
	virtual Pos printInfo( WINDOW* wnd, const Pos& tl, const Pos& br ) const { throw std::string("can't print info"); };
protected:
	Object() = default;
	Object( LAYER layer, OBJECT objT ) : m_layer{ layer }, m_objT{ objT }, m_power{0}{}
private:
	virtual void onTileChange( const Tile_w& oldTile, const Tile_p& newTile ) {}
	bool m_inAnimation{false};
	LAYER m_layer {LAYER::None};
	OBJECT m_objT {OBJECT::None};
	std::weak_ptr<Tile> m_tile {};
	std::size_t m_power{0};
};
class Tile : public std::enable_shared_from_this<Tile> {
public:
	Tile( bool canStep = true, bool promotionZone = false );
	Tile_p self() { return shared_from_this(); }
	void draw(WINDOW* wnd, const Pos& pos, const Pos& size); 
	static void link(
			const Pos& dir, const Tile_p& from, const Tile_p& to 
		);
	void addObject(const Obj_p& obj) ;
	void removeObject(Object& obj) ;
	const Tile_p& getNeighbor( const Pos& dir);
	const Obj_p& getLayer( const LAYER layer ) const;
	const Obj_p& getObjet() const { return getLayer( LAYER::Object ); }
	STEPS tryMove( const Object& obj ) const;
	void printInfo( WINDOW* wnd ) const;
	void block( bool block = true ) { 
		if ( block == m_canStep ) {
			m_canStep = !m_canStep;
			Obj_p obj = m_alternativeBg;
			decltype( m_objs )::iterator bg = m_objs.find( LAYER::Background );
			if ( bg == m_objs.end() ) {
				throw std::string( "no background layer defined to swap with!" );
			}
			m_objs.erase( bg );
			m_objs.insert(std::move(obj) );
		}
	}
	void setAlternativeBack( const Obj_p& obj ) {
		if ( obj->getLayer() != LAYER::Background ) {
			throw std::string("only accepts Background Objects!");
		}
		m_alternativeBg = obj;
	}
	bool isPromotionZone() const { return m_promtionZone; }
	bool canStap() const { return m_canStep; }
private:
	bool m_promtionZone{ false };
	bool m_canStep{true};
	Obj_p m_alternativeBg{};
	std::set<Obj_p, Object::less> m_objs;
	std::array<Tile_p, 9> m_neighbors = {
		nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr
	};
};

