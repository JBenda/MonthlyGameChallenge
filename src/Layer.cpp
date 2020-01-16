#include "Layer.hpp"

void Selected::draw( WINDOW* wnd, const Pos& pos, const Pos& size ) {
	static const int format = Glob::instance().GetColorAttrib( COLOR_BLACK, COLOR_BLUE );
	wattron( wnd, format );
	for ( int y = 0; y < size[1]; ++y ) {
		wmove( wnd, pos[1] + y, pos[0] );
		for ( int x = 0; x < size[0]; ++x ) {
			waddch( wnd, ' ' );
		}
	}
	wattroff( wnd, format );
}
