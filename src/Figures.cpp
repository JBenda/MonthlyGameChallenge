#include "Figures.hpp"

void Figure::draw(WINDOW* wnd, const Pos& pos, const Pos& size) {
	wmove( wnd, pos[1], pos[0] );
	int format = winch( wnd ) & A_COLOR;
	wattron( wnd, format );
	int height = size[1];
	const auto area = this->getPrint( height );
	for ( auto itr = area.first; itr != area.second; ++itr ) {
		int xOff = (size[0] - itr->size()) / 2;
		wmove( wnd, pos[1] + size[1] - itr.level(), pos[0] + xOff);
		waddstr( wnd, reinterpret_cast<const char*>(itr->data()) );
	}

	wattroff( wnd, format );
}

Figure::PrintArea Pawn::getPrint( const int height ) {
	if ( height > 5 ) {
		static const char8_t p[] =
			u8"()\0"
			u8")(\0"
			u8"/__\\\0"
			u8"";
		return PrintIterator::fromPrint( { p, sizeof( p ) } );

	}
	return PrintIterator::fromPrint(u8"\u2659");
}

Figure::PrintArea Figure::PrintIterator::fromPrint( const std::u8string_view& print ) {
	return std::make_pair(PrintIterator(print), PrintIterator());
}

Figure::PrintIterator& Figure::PrintIterator::operator++() {
	if ( m_msg.size() == 0 ) {
		m_lvl = -1;
		return *this;
	}
	std::size_t p = m_msg.find_last_of( u8'\0' );
	if ( p == std::u8string_view::npos ) {
		m_now = m_msg;
		m_msg.remove_suffix( m_now.size() );
	} else { 
		m_now = m_msg.substr( p + 1 ); 
	m_msg.remove_suffix( m_now.size() + 1);
	}
	++m_lvl;
	return *this;
}
