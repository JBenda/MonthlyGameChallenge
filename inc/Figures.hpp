#pragma once

#include "config.hpp"
#include "Board.hpp"

#include <vector>
#include <memory>

class PowerUp;
using Power_p = std::shared_ptr<PowerUp>;


class Figure : public Object{
	std::string_view getFractionName( FRACTION fraction ) const {
		switch ( fraction ) {
		case FRACTION::NORMAL:
			return "Normal";
		case FRACTION::PLAYER:
			return "Player";
		default:
			throw std::string("fraction has no Name!");
		}
	}
	short getFractionColor( FRACTION fraction ) const {
		switch ( fraction ) {
		case FRACTION::NORMAL:
			return COLOR_RED;
		case FRACTION::PLAYER:
			return COLOR_GREEN;
		default:
			throw std::string("fraction has no color!");
		}
	}
public:
	Figure( FRACTION fraction ) : Object{ LAYER::Object, OBJECT::Figure }, _fraction{fraction}{}
	void draw(WINDOW* wnd, const Pos& pos, const Pos& size) override;
	const std::vector<Tile_w>& getMovments();
	/// @brief used for KI
	virtual Tile_w getMove();
	FRACTION getFraction() const { return _fraction; }
	bool onCollision( const Obj_p& obj ) override;
	void addPowerUp( const Power_p& powerup );
	void removePowerUp( const Power_p& powerup );
	Pos printInfo( WINDOW* wnd, const Pos& tl, const Pos& br ) const override;
protected:
	Tile_w extraxtGreedeMove( const std::vector<Tile_w>& moves );
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
	virtual std::u8string_view getPrint( const Pos& size ) const = 0;
	virtual void setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const = 0;
	virtual std::string_view getName() const = 0;
	virtual std::string_view description() const { return ""; }
	FRACTION _fraction{ FRACTION::NORMAL };
	std::vector<Tile_w> m_targets{};
	std::vector<Power_p> m_powerups{};
};

class Pawn : public Figure {
public:
	Pawn( FRACTION fraction ) : Figure( fraction ) {}
private:
	std::string_view getName() const override { return m_queen ? "Queen" : "Pawn";  }
	std::string_view description() const override { return m_queen ? "" : "evoles to queen when reached last row."; }
	void setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const override;
	std::u8string_view getPrint( const Pos& size ) const override;
	void onTileChange( const Tile_w& oldTile, const Tile_p& newTile ) override;
	bool m_queen{ false };
};

class Bishop : public Figure {
public:
	Bishop( FRACTION fraction ) : Figure( fraction ) {}
private:
	std::string_view getName() const override { return "Bishop"; }
	void setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const override;
	std::u8string_view getPrint( const Pos& size ) const override;
};

class King : public Figure {
public:
	using Figure::Figure;
private:
	std::string_view getName() const override { return "King"; }
	void setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const override;
	std::u8string_view getPrint( const Pos& size ) const override;
};

class Knight : public Figure {
public:
	using Figure::Figure;
private:
	std::string_view getName() const override { return "Knight"; }
	void setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const override;
	std::u8string_view getPrint( const Pos& size ) const override;
};

class Tower : public Figure {
public:
	using Figure::Figure;
private:
	std::string_view getName() const override { return "Tower"; }
	void setMovments( const Tile_p& tile, std::vector<Tile_w>& movList ) const override;
	std::u8string_view getPrint( const Pos& size ) const override;
};

