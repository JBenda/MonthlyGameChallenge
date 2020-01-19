#include "Game.hpp"
#include "Figures.hpp"
#include "Layer.hpp"
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
			Tile_p tile = (new Tile())->self();
			if (x == 0) { // left border
				tile->addObject(std::make_shared<BgLabel>(Nums[y]));
			} else if (y == 0) { // right border
				tile->addObject(std::make_shared<BgLabel>(Alphs[x]));
			} else { // field
				if ((x+y) % 2) {
					tile->addObject(std::make_shared<BgColor>(COLOR_BLACK));
				} else {
					tile->addObject(std::make_shared<BgColor>(COLOR_WHITE));
				}
			}
			board.setTile({x,y}, tile);
		}
	}

}

void Game::draw() {
	m_board->draw(m_boardWnd);
	wnoutrefresh(m_boardWnd);
	wnoutrefresh(m_wnd);
}

void Game::update(const std::chrono::duration<float>& dt) {}

bool Game::running() { 
	return !m_exit;
}

constexpr Pos TileSize{14,7};

Game::Game(WINDOW* wnd) : 
	m_exit(false), 
	m_board(std::make_unique<Board>(TileSize)), 
	m_wnd{wnd},
	m_selector{ std::make_shared<Selected>() }{
	const Pos wndSize = getWndSize(wnd);
	if(wndSize[0] < TileSize[0]*9 + 10 || wndSize[1] < TileSize[1]*9+10) 
		throw std::string("Window to small");

	m_boardWnd = subwin(m_wnd, TileSize[1]*9,TileSize[0]*9, 5, 10); 
	loadMap(*m_board);

	m_board->getTile({4,4}).addObject(std::make_shared<Pawn>());
	m_board->getTile( { 1,1 } ).addObject( m_selector );
}

void Game::input(const Msg& msg) {
	if(const int* key = msg.fetch<int>(MsgTypes::Key)) {
		if (*key == 'q') {
			m_exit = true;
		} else if ( *key == KEY_RIGHT ) {
			Board::tryMove(m_selector, Directions.right);
		} else if (*key == KEY_LEFT) {
			Board::tryMove(m_selector, Directions.left);
		} else if ( *key == KEY_UP) {
			Board::tryMove(m_selector, Directions.up);
		} else if ( *key == KEY_DOWN) {
			Board::tryMove(m_selector, Directions.down);
		}

	}
	// TODO: distribute to sub elements
}
