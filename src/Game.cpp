#include "Game.hpp"
#include "Tiles.hpp"
#include "Figures.hpp"
#include <ncurses.h>
#include <array>
#include <string_view>

constexpr std::array<std::string_view, 20> Nums = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19"
};

constexpr std::array<std::string_view, 20> Alphs = {
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", 
	"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T" 
};

void loadMap(Board& board) {
	for(int x = 0; x < 9; ++x) {
		for(int y = 0; y < 9; ++y) {
			std::shared_ptr<Tile> tile(nullptr);
			if (x == 0) { // left border
				tile = std::make_shared<LabelTile>(Nums[y]);
			} else if (y == 0) { // right border
				tile = std::make_shared<LabelTile>(Alphs[x]);
			} else { // field
				if ((x+y) % 2) {
					tile = std::make_shared<BlackTile>();
				} else {
					tile = std::make_shared<WhiteTile>();
				}
			}
			board.setTile({x,y}, tile);
		}
	}

	board.getTile({4,4}).addObject(std::make_shared<Pawn>());
}

void Game::draw() {
	m_board->draw();
}

void Game::update(const std::chrono::duration<float>& dt) {}

bool Game::running() { 
	return true;
}

Game::Game() : m_board(std::make_unique<Board>()){
	loadMap(*m_board);
	auto obj = std::make_shared<Object>();
	m_board->setObject({0,0}, obj);
}
