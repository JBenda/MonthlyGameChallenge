#pragma once

#include <chrono>
#include <memory>

#include "Animator.hpp"
#include "Board.hpp"
#include "Layer.hpp"
#include "Msg.hpp"

class Figure;

class Game {
public:
	Game(WINDOW* wnd);
	void draw();
	void update(const std::chrono::duration<float>& dT);
	bool running();
	void input(const Msg& msg);
private:
	void handleSelection();
	void updateSelection();
	void flushSelectedFigure();
	bool tryMoveFigure( const std::shared_ptr<Figure>& fig, const Tile_p& tile );

	bool acceptInput() const { return !m_animator.inQueue(); }

	std::vector<std::shared_ptr<Marked>> m_moves; //< moves which are allowed at the moment
	std::shared_ptr<Figure> m_seletedFigure;

	bool m_exit{false};
	Animator m_animator;
	std::unique_ptr<Board> m_board;
	WINDOW* m_wnd{nullptr};
	WINDOW* m_boardWnd{nullptr};
	WINDOW* m_infoWnd{ nullptr };
	Pos m_tileSize{1,1};
	std::shared_ptr<Selected> m_selector{nullptr};
};
