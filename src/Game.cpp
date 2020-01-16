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

}

void Game::draw() {
	m_board->draw(m_boardWnd);
	wrefresh(m_wnd);
}

void Game::update(const std::chrono::duration<float>& dt) {}

bool Game::running() { 
	return !m_exit;
}

constexpr Pos TileSize{6,3};

Game::Game(WINDOW* wnd) : m_exit(false), m_board(std::make_unique<Board>(TileSize)), m_wnd{wnd}{
	const Pos wndSize = getWndSize(wnd);
	if(wndSize[0] < 6*9 + 10 || wndSize[1] < 3*9+10) throw std::string("Window to small");
	m_boardWnd = subwin(m_wnd, 3*9,6*9, 10, 10); 
	loadMap(*m_board);

	m_board->getTile({4,4}).addObject(std::make_shared<Pawn>());
}

void Game::input(const Msg& msg) {
	if(const int* key = msg.fetch<int>(MsgTypes::Key)) {
		if (*key == 'q') {
			m_exit = true;
		}
	}
	// TODO: distribute to sub elements
}
