#include "Game.hpp"
#include "Figures.hpp"
#include "Layer.hpp"
#include "PowerUp.hpp"
#include <array>
#include <string_view>

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
			Tile_p tile = std::make_shared<Tile>(x != 0 && y != 0);
			if (x == 0) { // left border
				tile->addObject(std::make_shared<BgLabel>(Nums[y]));
			} else if (y == 0) { // right border
				tile->addObject(std::make_shared<BgLabel>(Alphs[x]));
			} else { // field
				if ((x+y) % 2) {
					tile->addObject(std::make_shared<BgColor>(COLOR_MAGENTA));
				} else {
					tile->addObject(std::make_shared<BgColor>(COLOR_WHITE));
				}
			}
			board.setTile({x,y}, tile);
		}
	}

}

void loadPlayerSetUp( Board& board ) {
	board.getTile( { 4,8 } )->addObject( std::make_shared<King>( FRACTION::PLAYER ) );
	board.getTile( { 3,8 } )->addObject( std::make_shared<Bishop>( FRACTION::PLAYER ) );
	board.getTile( { 5,8 } )->addObject( std::make_shared<Knight>( FRACTION::PLAYER ) );
	board.getTile( { 6,8 } )->addObject( std::make_shared<Bishop>( FRACTION::PLAYER ) );
	for ( int i = 3; i <= 6; ++i ) {
		board.getTile( { i, 7 } )->addObject( std::make_shared<Pawn>( FRACTION::PLAYER ) );
	}
}

void Game::draw() {
	m_board->draw(m_boardWnd);
	m_animator.draw( m_boardWnd );
	m_selector->getTile()->printInfo( m_infoWnd );
	wnoutrefresh(m_boardWnd);
	wnoutrefresh( m_infoWnd );
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
	if ( wndSize[0] - boardWinSize[0] < 20 ) {
		infoWinPos = { 0,0 };
		infoWinSize = boardWinSize;
	} else {
		infoWinPos = { boardWinSize[0], 0 };
		infoWinSize = { wndSize[0] - boardWinSize[0], boardWinSize[1] };
	}

	m_boardWnd = subwin(m_wnd, boardWinSize[1],boardWinSize[0], 0,0); 
	m_infoWnd = subwin( m_wnd, infoWinSize[1], infoWinSize[0], infoWinPos[1], infoWinPos[0] );

	m_board = std::make_unique<Board>(m_tileSize);
	loadMap(*m_board);
	loadPlayerSetUp( *m_board );
	m_selector = std::make_shared<Selected>();
	m_board->getTile( { 4,4 } )->addObject( m_selector );

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
			std::chrono::milliseconds( static_cast<int>(std::sqrt(sqDistance)) * 40  ), fig ) );
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
	for ( const auto& wpFigure : m_nonPlayerFigures ) {
		if ( !wpFigure.expired() ) {
			std::shared_ptr<Figure> figure = wpFigure.lock();
			auto target = figure->getMove();
			if ( !target.expired() ) {
				moveFigure( figure, target.lock() );
			}
		}
	}
}
