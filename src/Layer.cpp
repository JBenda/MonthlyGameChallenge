#include "Layer.hpp"

void Selected::draw( WINDOW* wnd, const Pos& pos, const Pos& size ) {
	static const int format = Glob::instance().GetColorAttrib( COLOR_BLACK, COLOR_BLUE );
	wattron( wnd, format );
	for ( int y = 0; y < size[1]; ++y ) {
		wmove( wnd, pos[1] + y, pos[0] );
		for ( int x = 0; x < size[0]; ++x ) {
			if ( y == 0 || y == size[1] - 1 ) {
				waddch( wnd, ' ' );
			} else {
				waddstr( wnd, "  " );
				wmove( wnd, pos[1] + y, pos[0] + size[0] - 2 );
				waddstr( wnd, "  " );
				break;
			}
		}
	}
	wattroff( wnd, format );
}

void Marked::draw( WINDOW* wnd, const Pos& pos, const Pos& size ) {
	static const int format = Glob::instance().GetColorAttrib( COLOR_BLACK, COLOR_YELLOW );
	wattron( wnd, format );
	for ( int y = 1; y < size[1] - 1; ++y ) {
		wmove( wnd, pos[1] + y, pos[0] + 2);
		for ( int x = 2; x < size[0] - 2; ++x ) {
			waddch( wnd, ' ' );
		}
	}
	wattroff( wnd, format );
}

void BgColor::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	wattron(wnd, m_format);
	for(int y = 0; y < size[1]; ++y) {
		wmove(wnd, pos[1] + y, pos[0]);
		for(int x = 0; x < size[0]; ++x) {
			waddch(wnd, ' ');
		}
	}
	wattroff(wnd, m_format);
}
void BgLabel::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	const int begin = -static_cast<int>(m_msg.size()) / 2, end = begin + m_msg.size();
	int wp = -(size[0] * size[1] / 2) - 1;
	for(int y = 0; y < size[1]; ++y) {
		wmove(wnd, pos[1]  + y, pos[0]);
		for(int x = 0; x < size[0]; ++x) {
			if (++wp >= begin && wp < end ) {
				waddch(wnd, m_msg[wp - begin]);
			} else {
				waddch(wnd, ' ');
			}	
		}
	}
}
