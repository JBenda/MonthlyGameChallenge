#include "Tiles.hpp"

#include <ncurses.h>

void WhiteTile::drawSelf(const Pos& pos, const Pos& size) {
	static int format = Glob::instance().GetColorAttrib(COLOR_BLACK, COLOR_WHITE); 
	attron(format);
	for(int y = 0; y < size[1]; ++y) {
		move(pos[1] + y, pos[0]);
		for(int x = 0; x < size[0]; ++x) {
			addch(' ');
		}
	}
	attroff(format);
}

void BlackTile::drawSelf(const Pos& pos, const Pos& size) {
	static short format = Glob::instance().GetColorAttrib(COLOR_WHITE, COLOR_BLACK); 
	attron(format);
	for(int y = 0; y < size[1]; ++y) {
		move(pos[1] + y, pos[0]);
		for(int x = 0; x < size[0]; ++x) {
			addch(' ');
		}
	}
	attroff(format);
}

LabelTile::LabelTile(std::string_view msg) : m_msg(std::move(msg)){}

void LabelTile::drawSelf(const Pos& pos, const Pos& size) {
	const int begin = -m_msg.size() / 2, end = begin + m_msg.size();
	int wp = -(size[0] * size[1] / 2) - 1;
	for(int y = 0; y < size[1]; ++y) {
		move(pos[1]  + y, pos[0]);
		for(int x = 0; x < size[0]; ++x) {
			if (++wp >= begin && wp < end ) {
				addch(m_msg[wp - begin]);
			} else {
				addch(' ');
			}	
		}
	}
}
