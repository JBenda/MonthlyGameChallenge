#include "Figures.hpp"

void Pawn::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	wmove(wnd, pos[1] + size[1] / 2, pos[0] + size[0] / 2);
	short color;
	wprintw(wnd, "\u2659");
}
