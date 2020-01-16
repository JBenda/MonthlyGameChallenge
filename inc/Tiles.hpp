#pragma once

#include <string_view>

#include "Board.hpp"
#include "config.hpp"

class WhiteTile : public Tile {
	void drawSelf(WINDOW* wnd, const Pos& pos, const Pos& size) override;
};

class BlackTile : public Tile {
	void drawSelf(WINDOW* wnd, const Pos& pos, const Pos& size) override;
};

class LabelTile : public Tile {
	void drawSelf(WINDOW* wnd, const Pos& pos, const Pos& size) override;
public:
	LabelTile(std::string_view msg);
private:
	std::string_view m_msg;
};
