#pragma once

#include <chrono>
#include <memory>

#include "Board.hpp"
#include "Msg.hpp"

class Game {
public:
	Game(WINDOW* wnd);
	void draw();
	void update(const std::chrono::duration<float>& dT);
	bool running();
	void input(const Msg& msg);
private:
	bool m_exit{false};
	std::unique_ptr<Board> m_board;
	WINDOW* m_wnd{nullptr};
	WINDOW* m_boardWnd{nullptr};
};
