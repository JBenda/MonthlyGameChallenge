#pragma once
#include "config.hpp"
#include "Board.hpp"

class Selected : public Object {
public:
	void draw( WINDOW* wnd, const Pos& pos, const Pos& size ) override;
	Selected() : Object( LAYER::Highlight, OBJECT::Highlight ) {}
	~Selected() = default;
};

class Marked : public Object {
public:
	void draw( WINDOW* wnd, const Pos& pos, const Pos& size ) override;
	Marked() :
		Object( LAYER::Marked, OBJECT::Highlight ) {}
};

class BgColor : public Object {
public:
	void draw( WINDOW* wnd, const Pos& pos, const Pos& size ) override;
	BgColor(short color) : 
		Object(LAYER::Background, 
		OBJECT::Highlight ),
		m_format(Glob::instance().GetColorAttrib(COLOR_GREEN, color)){}
private:
	const int m_format;
};


class BgLabel : public Object {
public:
	void draw( WINDOW* wnd, const Pos& pos, const Pos& size ) override;
	BgLabel(std::string_view msg) : Object(LAYER::Background, OBJECT::Highlight), m_msg{std::move(msg)} {}
private:
	const std::string_view m_msg;
};
