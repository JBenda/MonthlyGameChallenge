#pragma once

#include <chrono>
#include <memory>

#include "Board.hpp"

class Game {
public:
	Game();
	void draw();
	void update(const std::chrono::duration<float>& dT);
	bool running();
private:
	std::unique_ptr<Board> m_board;
};
