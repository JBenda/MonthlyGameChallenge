#pragma once

#include "Board.hpp"
#include <memory>

class Figure;
class PowerUp;
using Power_p = std::shared_ptr<PowerUp>;

class PowerUp : public Object, public std::enable_shared_from_this<PowerUp> {
public:
	PowerUp() : std::enable_shared_from_this<PowerUp>(), Object{ LAYER::Object, OBJECT::PowerUp }{}
	virtual bool onCollision( const Obj_p& obj ) override { return true; }
	virtual void modifyMovments( std::vector<Tile_w>& moves ) {}
	virtual std::u8string_view getPrint(const Pos& size) = 0;
	Pos printInfo( WINDOW* wnd, const Pos& tl, const Pos& br ) const ;
protected:
	Power_p self() { return shared_from_this(); }
private:
	virtual std::string_view getDescription() const = 0;
	virtual std::string_view getName() const = 0;
};

class Konter : public PowerUp {
public:
	using PowerUp::PowerUp;
	bool onCollision( const Obj_p& obj ) override;
	std::u8string_view getPrint( const Pos& size ) override;
	void draw( WINDOW* wnd, const Pos& pos, const Pos& size ) override;
private:
	std::string_view getDescription() const override { return "Next close range attack will be conter."; }
	std::string_view getName() const override { return "Conter"; }
	std::weak_ptr<Figure> m_figure{};
};
using Power_p = std::shared_ptr<PowerUp>;
