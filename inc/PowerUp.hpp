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
protected:
	Power_p self() { return shared_from_this(); }
};

class Konter : public PowerUp {
public:
	using PowerUp::PowerUp;
	bool onCollision( const Obj_p& obj ) override;
private:
	std::weak_ptr<Figure> m_figure{};
};
using Power_p = std::shared_ptr<PowerUp>;
