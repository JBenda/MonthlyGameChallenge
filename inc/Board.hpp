#pragma once

#include <unordered_map>
#include <array>
#include <memory>
#include <vector>
#include <string>

#include "config.hpp"


template<typename T, std::size_t N>
struct Vec : public std::array<T,N>{
	Vec<T,N>& operator+=(Vec<T,N>& v) {
		for(std::size_t i = 0; i < N; ++i) {
			*this[i] += v[i];
		}
		return *this;
	}
	Vec<T,N>& operator-=(Vec<T,N>& v) {
		for(std::size_t i = 0; i < N; ++i) {
			*this[i] -= v[i];
		}
		return *this;
	}
	Vec<T,N>& operator*=(Vec<T,N>& s) {
		for(std::size_t i = 0; i < N; ++i) {
			*this[i] *= s;
		}
		return *this;
	}
	Vec<T,N> operator+(Vec<T,N>& v) const {
		Vec<T,N> res;
		for(std::size_t i = 0; i < N; ++i) {
			res[i] = *this[i] + v[i]; 
		}
	}
	Vec<T,N> operator-(Vec<T,N>& v) const {
		Vec<T,N> res;
		for(std::size_t i = 0; i < N; ++i) {
			res[i] = *this[i] + v[i];
		}
	}
	Vec<T,N> operator*(T s) const {
		Vec<T,N> res;
		for(std::size_t i = 0; i < N; ++i) {
			res[i] = *this[i] * s;
		}
	}

	std::string str() const {
		std::string res = "[";
		for(std::size_t i = 0; i < N; ++i) {
			if(i != 0) { res += ','; }
			res += std::to_string(*this[i])	;
		}	
	}
};

template<typename T, std::size_t N>
class std::hash<Vec<T,N>> {
	
};

using Pos = Vec<int16_t, 2>;

class Object {
	virtual void onChangePos(
			const Pos oPos,
			const Pos nPos) {}
public:
	virtual void draw() const;
	void setPos(const Pos pos) {
		onChangePos(m_pos, pos);
		m_pos = pos;
	}
	const Pos& getPos() const {
		return m_pos;
	}
private:
	Pos m_pos;
};
using obj_ptr = std::shared_ptr<Object>;
using obj_ref = const obj_ptr&;

class Tile {
	virtual void onEnter(obj_ref obj) { }
public:
	void addObject(obj_ref obj) {
		onEnter(obj);
		m_objs.emplace_back(obj);
	}
private:
	std::vector<obj_ptr> m_objs;
};

class Board {
	using Map_t =
		std::unordered_map<Pos, std::shared_ptr<Tile>>;

protected:
	Tile& getTile(const Pos& pos) {
		auto itr = m_map->find(pos);
		if(itr != m_map.end()) return itr->second;
		throw std::string("Try access not existing tile");
	}

public:
	void setObject(
			const Pos pos,
			obj_ref obj) {
		getTile(pos).addObject(obj);
	}
	void setTile(
			const Pos pos,
			const Tile& tile){
		if(!m_map->emplace(tile).second) {
			throw std::string("Tile: ") + pos.str() + " already tiled.";
		}
	}

private:
	Map_t m_map;
};
