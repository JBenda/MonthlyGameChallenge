#pragma once

#include "config.hpp"
#include "Board.hpp"

#include <vector>


class Figure : public Object{
public:
	Figure() : Object{LAYER::Object, OBJECT::Figure}{}
	void draw(WINDOW* wnd, const Pos& pos, const Pos& size) override;
	const std::vector<Tile_w>& getMovments();
protected:

	class PrintIterator;
	using PrintArea = std::pair<PrintIterator, PrintIterator>;
	class PrintIterator {
	public:
		static PrintArea fromPrint( const std::u8string_view& print );
		PrintIterator& operator++();
		bool operator!=( const PrintIterator& itr ) { return m_lvl != itr.m_lvl; }
		const std::u8string_view& operator*() { return m_now; }
		const std::u8string_view* operator->() { return &m_now; }
		int level() { return m_lvl; }
	private:
		PrintIterator() = default;
		PrintIterator( const std::u8string_view& print )
			: m_msg( print ),
			  m_lvl{ 0 } {
			m_msg.remove_suffix(1);
			++( *this );
		}
		std::u8string_view m_msg;
		std::u8string_view m_now;
		int m_lvl{ -1 };
	};
private:
	virtual PrintArea getPrint( int const height ) = 0;
	virtual void setMovments( std::vector<Tile_w>& movList ) const = 0;
	std::vector<Tile_w> m_targets;
};

class Pawn : public Figure {
public:
private:
	void setMovments( std::vector<Tile_w>& movList ) const override;
	PrintArea getPrint( const int height ) ;
};

