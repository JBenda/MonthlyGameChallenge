#pragma once

#include "config.hpp"
#include "Board.hpp"

class Figure : public Object{
public:
	Figure() : Object{LAYER::Object, OBJECT::Figure}{}
};

class Pawn : public Figure {
public:
	void draw(WINDOW* wnd, const Pos& pos, const Pos& size) override;
};
