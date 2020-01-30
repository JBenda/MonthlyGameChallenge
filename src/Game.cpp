#include "Game.hpp"
#include "Figures.hpp"
#include "Layer.hpp"
#include "PowerUp.hpp"
#include <array>
#include <string_view>
#include <random>

constexpr std::array<std::string_view, 20> Nums = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19"
};

constexpr std::array<std::string_view, 20> Alphs = {
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", 
	"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T" 
};

void loadMap(Board& board) {
	for(int x = 0; x < 9; ++x) {
		for(int y = 0; y < 9; ++y) {
			Tile_p tile = std::make_shared<Tile>(x != 0 && y != 0, y == 1);
			if (x == 0) { // left border
				tile->addObject(std::make_shared<BgLabel>(Nums[y]));
			} else if (y == 0) { // right border
				tile->addObject(std::make_shared<BgLabel>(Alphs[x]));
			} else { // field
				if ((x+y) % 2) {
					tile->addObject(std::make_shared<BgColor>(COLOR_MAGENTA));
				} else {
					tile->addObject(std::make_shared<BgColor>(COLOR_BLACK));
				}
			}
			board.setTile({x,y}, tile);
		}
	}

}

void loadPlayerSetUp( Board& board, std::weak_ptr<Figure>& king ) {
	auto k = std::make_shared<King>( FRACTION::PLAYER );
	king = k;
	board.getTile( { 4,8 } )->addObject( k );
	board.getTile( { 3,8 } )->addObject( std::make_shared<Bishop>( FRACTION::PLAYER ) );
	board.getTile( { 5,8 } )->addObject( std::make_shared<Knight>( FRACTION::PLAYER ) );
	board.getTile( { 6,8 } )->addObject( std::make_shared<Bishop>( FRACTION::PLAYER ) );
	for ( int i = 3; i <= 6; ++i ) {
		board.getTile( { i, 7 } )->addObject( std::make_shared<Pawn>( FRACTION::PLAYER ) );
	}
}

void placeRandomFiguren( Board& board, float count, FRACTION fraction, std::vector<std::weak_ptr<Figure>>* list = nullptr) {
	static std::mt19937 generator(time(NULL));
	std::uniform_real_distribution distribution{};
	std::vector<std::pair<Pos, float>> spawnProp;
	for ( const std::pair<Pos, Tile_p>& tile : board ) {
		if ( tile.second->canStap() && !( tile.second->getObjet() ) ) {
			float p = 1.f;
			for ( const Pos& dir : Directions ) {
				if ( const Tile_p& next = tile.second->getNeighbor(dir) ) {
					if ( next->getObjet() ) { p -= 0.2f; }
					for ( const Pos& subD : Directions ) {
						if ( const Tile_p& nnext = next->getNeighbor( subD ) ) {
							if ( nnext->getObjet() ) { p -= 0.1f; }
						}
					}
				}
			}
			spawnProp.push_back( { tile.first, p} );
		}
	}
	float sum = 0;
	for ( auto& entry : spawnProp ) { sum += std::max( 0.f, entry.second ); }
	int pices = 0;
	int powerUps = 1;
	do {
		float mod = (count - static_cast<float>(pices))/ sum;
		for ( const auto& entry : spawnProp ) {
			const Tile_p& tile = board.getTile( entry.first );
			if ( !tile->getObjet() && distribution( generator ) < static_cast<double>( entry.second )* mod ) {
				if ( powerUps > 0 ) {
					tile->addObject( std::make_shared<Konter>() );
					--powerUps;
				} else {
					using Figs = std::tuple<Tower, Knight, Bishop>;
					int idx = distribution( generator ) * std::tuple_size_v<Figs>;
					std::shared_ptr<Figure> fig;
					switch ( idx ) {
					case 0: fig = std::make_shared<std::tuple_element_t<0, Figs>>( fraction ); break;
					case 1: fig = std::make_shared<std::tuple_element_t<1, Figs>>( fraction ); break;
					case 2: fig = std::make_shared<std::tuple_element_t<2, Figs>>( fraction ); break;
					default:
						throw std::string( "unknown type!" );
					}
					tile->addObject( fig );
					++pices;
					if ( list ) {
						list->push_back( fig );
					}
				}
			}
			if ( pices > 1.5f * count ) break;
		}
	} while ( pices < count * 0.5f);
}

void Game::updateState() {
	wmove( m_stateWnd, 2, 3 );
	std::string str = std::string( "Floor: ") + std::to_string(m_level) + "\tEnemy stund for: "  + std::to_string( m_freezCount ) + " turns";
	wprintw( m_stateWnd, "%-50s",  str.c_str());
}

void drawGameOver( WINDOW* wnd ) {
	Pos size = getWndSize( wnd );
	Pos tl( ( size[0] - 74 ) / 2, ( size[1] - 8) / 2 );
	WINDOW* label = subwin( wnd, 10, 74, tl[1], tl[0]);
	wattron( label, A_BOLD | Glob::instance().GetColorAttrib( COLOR_RED, COLOR_BLACK ) );
	mvwaddstr( label, 0, 0, " _______  _______  _______  _______    _______           _______  _______ " );
	mvwaddstr( label, 1, 0, "(  ____ \\(  ___  )(       )(  ____ \\  (  ___  )|\\     /|(  ____ \\(  ____ )" );
	mvwaddstr( label, 2, 0, "| (    \\/| (   ) || () () || (    \\/  | (   ) || )   ( || (    \\/| (    )|" );
	mvwaddstr( label, 3, 0, "| |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|" );
	mvwaddstr( label, 4, 0, "| | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __)" );
	mvwaddstr( label, 5, 0, "| | \\_  )| (   ) || |   | || (        | |   | | \\ \\_/ / | (      | (\\ (   " );
	mvwaddstr( label, 6, 0, "| (___) || )   ( || )   ( || (____/\\  | (___) |  \\   /  | (____/\\| ) \\ \\__" );
	mvwaddstr( label, 7, 0, "(_______)|/     \\||/     \\|(_______/  (_______)   \\_/   (_______/|/   \\__/" );
	mvwaddstr( label, 9, 30, "-- press q to quite --" );
	wnoutrefresh( label );
}

void Game::draw() {
	if (m_king.expired()) {
		drawGameOver( m_boardWnd );
	} else {
		m_board->draw( m_boardWnd );
		m_animator.draw( m_boardWnd );
		m_selector->getTile()->printInfo( m_infoWnd );
		updateState();
		wnoutrefresh(m_boardWnd);
		wnoutrefresh( m_infoWnd );
		wnoutrefresh( m_stateWnd );
	}
}

void Game::update(const std::chrono::duration<float>& dt) {
	m_animator.update( dt );
}

bool Game::running() { 
	return !m_exit;
}


Game::Game(WINDOW* wnd) : 
	m_exit(false), 
	m_wnd{wnd},
	m_selector{ std::make_shared<Selected>() }{
	const Pos wndSize = getWndSize(wnd);
	m_tileSize = Pos( (wndSize[0] - 10) / 9, (wndSize[1] - 5) / 9);
	for ( const Pos& size : SupportedTieldSizes ) {
		if ( size[0] <= m_tileSize[0] && size[1] <= m_tileSize[1] ) {
			m_tileSize = size;
			break;
		}
	}
	if(wndSize[0] < m_tileSize[0]*9 || wndSize[1] < m_tileSize[1]*9) 
		throw std::string("Window to small");
	const Pos boardSize( 9,9);
	const Pos boardWinSize( boardSize[0] * m_tileSize[0], boardSize[1] * m_tileSize[1] );
	Pos infoWinSize, infoWinPos;
	int stateHeight = 5;
	if ( wndSize[0] - boardWinSize[0] < 20 ) {
		infoWinPos = { 0,0 };
		infoWinSize = boardWinSize;
	} else {
		infoWinPos = { boardWinSize[0], stateHeight };
		infoWinSize = { wndSize[0] - boardWinSize[0], boardWinSize[1] };
	}

	m_stateWnd = subwin( m_wnd, stateHeight, wndSize[0], 0, 0 );
	box( m_stateWnd, '|', '-' );
	m_boardWnd = subwin(m_wnd, boardWinSize[1],boardWinSize[0], stateHeight,0); 
	m_infoWnd = subwin( m_wnd, infoWinSize[1], infoWinSize[0], infoWinPos[1], infoWinPos[0] );

	m_board = std::make_unique<Board>(m_tileSize);
	loadMap(*m_board);
	loadPlayerSetUp( *m_board, m_king );
	m_selector = std::make_shared<Selected>();
	m_board->getTile( { 4,4 } )->addObject( m_selector );
	nextFloor();

	wattron( m_boardWnd, A_BOLD );
}

void Game::nextFloor() {
	++m_level;
	placeRandomFiguren( *m_board, m_level * 2.f + 
						std::max(0.f, m_level - 2.f), FRACTION::NORMAL, &m_nonPlayerFigures );
	m_freezCount = m_level + 1;
}

void Game::flushSelectedFigure() {
	m_seletedFigure.reset();
	for ( const std::shared_ptr<Marked>& m : m_moves ) {
		m->getTile()->removeObject( *m );
	}
	m_moves.resize(0);
}

void Game::updateSelection() {
	const Obj_p& obj = m_selector->getTile()->getObjet();
	if ( obj && obj->getObjectType() == OBJECT::Figure ) {
		flushSelectedFigure();
		auto pFigure = std::static_pointer_cast<Figure>( obj );
		m_seletedFigure = pFigure;
		const std::vector<Tile_w>& moves = pFigure->getMovments();
		for ( const Tile_w& tile : moves ) {
			m_moves.push_back( std::make_shared<Marked>() );
			tile.lock()->addObject( m_moves.back() );
		}
	}
}

void Game::handleSelection() {
	if ( !m_seletedFigure.expired() ) {
		auto pFigure = m_seletedFigure.lock();
		if ( pFigure->getFraction() == FRACTION::PLAYER && tryMoveFigure( pFigure, m_selector->getTile() ) ) {
			m_animator.addBarrier();
			autoMovments();
			m_animator.addBarrier();
			flushSelectedFigure();
		} else {
			flushSelectedFigure();
			updateSelection();
		}
	} else {
		updateSelection();
	}
}

void Game::input(const Msg& msg) {
	if(const int* key = msg.fetch<int>(MsgTypes::Key)) {
		if (*key == 'q') {
			m_exit = true;
		} else if ( *key == KEY_RIGHT ) {
			Board::tryMove(m_selector, Directions.right);
			if ( m_seletedFigure.expired() || m_seletedFigure.lock()->getFraction() != FRACTION::PLAYER) {
				updateSelection();
			}
		} else if (*key == KEY_LEFT) {
			Board::tryMove(m_selector, Directions.left);
			if ( m_seletedFigure.expired() || m_seletedFigure.lock()->getFraction() != FRACTION::PLAYER) {
				updateSelection();
			}
		} else if ( *key == KEY_UP) {
			Board::tryMove(m_selector, Directions.up);
			if ( m_seletedFigure.expired() || m_seletedFigure.lock()->getFraction() != FRACTION::PLAYER) {
				updateSelection();
			}
		} else if ( *key == KEY_DOWN) {
			Board::tryMove(m_selector, Directions.down);
			if ( m_seletedFigure.expired() || m_seletedFigure.lock()->getFraction() != FRACTION::PLAYER) {
				updateSelection();
			}
		} else if ( *key == '\r' && acceptInput()) {
			handleSelection();
		}

	} else if ( const Pos* pos = msg.fetch<Pos>( MsgTypes::LeftClick ) ) {
		Pos winPos = *pos - getWndPos( m_boardWnd );
		Pos tileSize = m_board->getTileSize();
		const Tile_p& tile = m_board->getTile( { winPos[0] / tileSize[0], winPos[1] / tileSize[1] } );
		m_board->move( m_selector, *tile);
		handleSelection();
	}
}
bool Game::tryMoveFigure( const std::shared_ptr<Figure>& fig, const Tile_p& tile ) {
	for ( const std::shared_ptr<Marked>& mark : m_moves ) {
		if ( mark->getTile() == tile ) {
			moveFigure( fig, tile );
			return true;
		}
	}
	return false;
}

void Game::moveFigure( const std::shared_ptr<Figure>& fig, const Tile_p& tile ) {
	Pos pFig = m_board->getPosition( *fig->getTile() ).scale( m_tileSize );
	Pos pTile = m_board->getPosition( *tile ).scale( m_tileSize );
	int sqDistance = ( pTile - pFig ).sqDistance();
	m_animator.addAnimation( 
		Animator::Animation( 
			{ pFig, m_tileSize }, 
			{ pTile , m_tileSize }, 
			std::chrono::milliseconds( static_cast<int>(std::sqrt(sqDistance)) * 20  ), fig ) );
	if ( auto pObj = tile->getObjet() ) {
		m_animator.addAnimation(
			Animator::Animation(
				{ pTile, m_tileSize },
				{ pTile, m_tileSize },
				std::chrono::milliseconds( 1 ),
				pObj  ));
	}
	m_board->move( fig, *tile );
}

void Game::autoMovments() {
	if ( m_freezCount > 0 ) { --m_freezCount; return; }
	std::vector<decltype( m_nonPlayerFigures )::iterator> expired{};
	for ( auto itr = m_nonPlayerFigures.begin(); itr != m_nonPlayerFigures.end(); ++itr ) {
		const std::weak_ptr<Figure>& wpFigure = *itr;
		if ( !wpFigure.expired() ) {
			std::shared_ptr<Figure> figure = wpFigure.lock();
			auto target = figure->getMove();
			if ( !target.expired() ) {
				moveFigure( figure, target.lock() );
			}
		} else {
			expired.push_back( itr );
		}
	}
	for ( auto itr = expired.rbegin(); itr != expired.rend(); ++itr ) {
		m_nonPlayerFigures.erase(*itr);
	}
	if ( m_nonPlayerFigures.empty() ) {
		nextFloor();
	}
}
