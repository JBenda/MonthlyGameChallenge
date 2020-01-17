#pragma once

#include <array>
#include <chrono>
#include <initializer_list>
#include <type_traits>
#include <memory>
#include <unordered_map>

#define NCURSES_MOUSE_VERSION
#include <curses.h>


template<>
struct std::hash<std::pair<short,short>> {
	std::size_t operator()(const std::pair<short,short>& value) const {
		const std::hash<short> hash{};
		return std::hash<std::size_t>()(hash(value.first) + hash(value.second));
	}
};
class Glob {
public:
	static Glob& instance() {
		if (!m_instance) {
			m_instance = std::unique_ptr<Glob>(new Glob());
		}
		return *m_instance;
	}
	/// managed color attribute ids
	int GetColorAttrib(short fg, short bg) {
		auto itr = m_colorPairs.find({fg,bg});
		if (itr == m_colorPairs.end()) {
			short pid = static_cast<short>(m_colorPairs.size()) + 1;
			init_pair(pid, fg, bg);
			itr = m_colorPairs.insert(std::pair<std::pair<short, short>,short>(
				{fg,bg}, pid)).first;
		}
		return COLOR_PAIR(itr->second);
	}
private:
	Glob() = default;
	static std::unique_ptr<Glob> m_instance;
	std::unordered_map<std::pair<short,short>,short> m_colorPairs;
};



constexpr float FrameDuration = 1.f / 10.f;
constexpr float MaxTimeFrame = 0.5f;

enum class LAYER { None, Background, Highlight, Object };
enum class OBJECT { None, Figure, Highlight };

template<typename T, std::size_t N>
struct Vec : public std::array<T,N>{
	template<typename ... Ns>
	constexpr Vec(Ns ... ns) : std::array<T,N>{{ns...}}{}
	Vec<T,N>& operator+=(Vec<T,N>& v) {
		for(std::size_t i = 0; i < N; ++i) {
			*this[i] += v[i];
		}
		return *this;
	}
	Vec<T,N>& operator-=(const Vec<T,N>& v) {
		for(std::size_t i = 0; i < N; ++i) {
			*this[i] -= v[i];
		}
		return *this;
	}
	Vec<T,N>& operator*=(const Vec<T,N>& s) {
		for(std::size_t i = 0; i < N; ++i) {
			*this[i] *= s;
		}
		return *this;
	}
	constexpr Vec<T,N> operator+(const Vec<T,N>& v) const {
		Vec<T,N> res = *this;
		add(res, v);
		return res;
	}
	constexpr Vec<T,N> operator-(const Vec<T,N>& v) const {
		Vec<T,N> res;
		sub(res, v);
		return res;
	}
	constexpr Vec<T,N> operator*(T s) const {
		Vec<T,N> res;
		scal(res, s);
		return res;
	}

	std::string str() const {
		std::string res = "[";
		for(std::size_t i = 0; i < N; ++i) {
			if(i != 0) { res += ','; }
			res += std::to_string(this->at(i))	;
		}	
		return res;
	}
private:
	template<std::size_t I = N-1>
	constexpr void add(Vec<T,N>& r, const Vec<T,N>& v) const {
		std::get<I>(r) += std::get<I>(v);
		if constexpr (I>0) {
			add<I-1>(r,v);
		}
	}
	template<std::size_t I = N-1>
	constexpr void sub(Vec<T,N>& r, const Vec<T,N>& v) const {
		std::get<I>(r) -= std::get<I>(v);
		if constexpr (I>0) {
			sub<I-1>(r,v);
		}
	}
	template<std::size_t I = N-1>
	constexpr void scal(Vec<T,N>& r, T s) const {
		std::get<I>(r) *= s;
		if constexpr (I>0) {
			scal<I-1>(r,s);
		}
	}
};

template<typename T, std::size_t N>
struct std::hash<Vec<T,N>> {
	std::size_t operator()(const Vec<T,N>& vec)	const {
		std::size_t hash = 0;
		for(std::size_t i = 0; i < N; ++i) {
			hash += std::hash<T>()(vec[i]);
		}
		return std::hash<std::size_t>()(hash);
	}
};

using Pos = Vec<int, 2>;

class NeighborsC {
public:
	/**
	 *	@brief maps neighbors to numbers 0to8.
	 *	0 1 2 
	 *	3 - 5  <- 4 = self
	 *	6 7 8
	 */
	static constexpr std::size_t toId(const Pos& pos) {
		return pos[0] + pos[1] * 3 + 4;
	}
	static constexpr std::size_t mirrow(std::size_t id) {
		return 8 - id;
	}
	static constexpr Pos up = {0,-1};
	static constexpr Pos left = {-1,0};
	static constexpr Pos right= {1,0} ;
	static constexpr Pos down = {0,1};
	static auto begin() { return m_pos.begin();}
	static auto end() { return m_pos.end();}
private:
	static constexpr std::array<Pos,8> m_pos
		= {{
			up,
			left,
			right,
			down,
			up + left,
			up + right,
			down + left,
			down + right
		}};
};
constexpr NeighborsC Directions{};

inline Pos getWndSize(WINDOW* wnd) {
	Pos wndSize;
	getmaxyx(wnd, wndSize[1], wndSize[0]);
	return wndSize;
}
