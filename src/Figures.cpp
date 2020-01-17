#include "Figures.hpp"

void Pawn::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	wmove(wnd, pos[1] + size[1] / 2, pos[0]);
	constexpr char snow[] = {(char)0xE2, (char)0x98, (char)0x83, '\0'};
	wprintw(wnd, snow);
}
