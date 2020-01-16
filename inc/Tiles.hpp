#pragma once

#include <string_view>

#include "Board.hpp"
#include "config.hpp"

class WhiteTile : public Tile {
	void drawSelf(const Pos& pos) override;
};

class BlackTile : public Tile {
	void drawSelf(const Pos& pos) override;
};

class LabelTile : public Tile {
	void drawSelf(const Pos& pos) override;
public:
	LabelTile(std::string_view msg);
private:
	std::string_view m_msg;
};
