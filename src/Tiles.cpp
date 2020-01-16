#include "Tiles.hpp"

#include <ncurses.h>

void WhiteTile::drawSelf(const Pos& pos) {
	static int format = Glob::instance().GetColorAttrib(COLOR_BLACK, COLOR_WHITE); 
	attron(format);
	const Pos& dim = getDimensions();
	for(int y = 0; y < dim[1]; ++y) {
		move(pos[1] * dim[1] + y, pos[0] * dim[0]);
		for(int x = 0; x < dim[0]; ++x) {
			addch(' ');
		}
	}
	attroff(format);
}

void BlackTile::drawSelf(const Pos& pos) {
	static short format = Glob::instance().GetColorAttrib(COLOR_WHITE, COLOR_BLACK); 
	attron(format);
	const Pos& dim = getDimensions();
	for(int y = 0; y < dim[1]; ++y) {
		move(pos[1] * dim[1] + y, pos[0] * dim[0]);
		for(int x = 0; x < dim[0]; ++x) {
			addch(' ');
		}
	}
	attroff(format);
}

LabelTile::LabelTile(std::string_view msg) : m_msg(std::move(msg)){}

void LabelTile::drawSelf(const Pos& pos) {
	const Pos& dim = getDimensions();
	const int begin = -m_msg.size() / 2, end = begin + m_msg.size();
	int wp = -(dim[0] * dim[1] / 2) - 1;
	for(int y = 0; y < dim[1]; ++y) {
		move(pos[1] * dim[1] + y, pos[0] * dim[0]);
		for(int x = 0; x < dim[0]; ++x) {
			if (++wp >= begin && wp < end ) {
				addch(m_msg[wp - begin]);
			} else {
				addch(' ');
			}	
		}
	}
}
