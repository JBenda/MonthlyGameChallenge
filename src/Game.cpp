#include "Game.hpp"
#include <ncurses.h>

void Game::draw() {
	m_board->draw();
}

void Game::update(const std::chrono::duration<float>& dt) {}

bool Game::running() { 
	return true;
}

Game::Game() : m_board(std::make_unique<Board>()){
	Tile_p tile = std::make_shared<Tile>();
	tile->setSelf(tile);
	m_board->setTile({0,0}, tile);
	auto obj = std::make_shared<Object>();
	m_board->setObject({0,0}, obj);
}
