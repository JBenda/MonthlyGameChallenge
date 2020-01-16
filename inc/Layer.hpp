#pragma once
#include "config.hpp"
#include "Board.hpp"

class Selected : public Object {
public:
	void draw( WINDOW* wnd, const Pos& pos, const Pos& size ) override;
	Selected() : Object( LAYER::Highlight, OBJECT::Highlight ) {}
};