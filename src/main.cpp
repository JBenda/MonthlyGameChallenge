
#include <iostream>
#include <chrono>
#include <thread>

#include <Windows.h>

#include <clocale>

#include "config.hpp"
#include "Game.hpp"
#include "Msg.hpp"

enum struct STATES { Input, Animation, Calculation, Exit };
std::unique_ptr<Glob> Glob::m_instance{nullptr};

int main(int argc, char *argv[])
{
	std::chrono::milliseconds timespan( 1000 );

	SetConsoleTitle("Happy gaming :)");
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect( console, &r );
	HANDLE cBuffer = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_FONT_INFO font;
	bool ret = GetCurrentConsoleFont( cBuffer, false, &font );
	MoveWindow( console, r.left, r.top, font.dwFontSize.X * 200, font.dwFontSize.Y * 80, TRUE );
	WINDOW* wnd = initscr();

	if (has_colors() == FALSE) {
		std::cerr << "You're terminal don't support colors, sorry\n";
	}
	start_color();

	mvaddstr(0,0, std::setlocale( LC_ALL, "" ));

	noecho();
	cbreak();
	keypad(wnd, TRUE);
	nonl();
	intrflush(wnd, FALSE);
	nodelay(wnd, TRUE);
	curs_set(FALSE);
	mousemask(BUTTON1_CLICKED, nullptr);

	init_color(COLOR_BLACK, 0,0,0);
	init_color( COLOR_BLUE, 678, 847, 902 );
	init_color( COLOR_WHITE, 888, 888, 888 );
	init_color( COLOR_MAGENTA, 111, 111, 111 );
	init_color( COLOR_RED, 999, 222, 222 );

	MsgQueue msgQueue{};

	bool closeNormal = true;
	try {
		Game game(wnd);

		using Clock = std::chrono::steady_clock;
		std::chrono::time_point<Clock> now;
		std::chrono::time_point<Clock> last;
		last = Clock::now();

		const std::chrono::duration<float> dt{FrameDuration};
		const std::chrono::duration<float> maxTime{MaxTimeFrame};

		std::chrono::duration<float> accumulator(0);
		Msg msg{};
		while (game.running()) {
			now = Clock::now();
			std::chrono::duration<float> frameTime = now - last;
			last = now;
			if(frameTime > maxTime) {
				frameTime = maxTime;
			} 

			accumulator += frameTime;


			bool changes = false;
			while( accumulator > (dt / 2.f) ) {
				changes = true;
				game.update(dt);
				accumulator -= dt;
			}
			if (changes) {
				msgQueue.fetchEvents( wnd );
				while(msgQueue.pop(msg)) {	
					game.input(msg);
				}
				game.draw();
				wnoutrefresh(wnd);
				doupdate();
			}

		}
	} catch ( const std::string& msg) {
		closeNormal = false;
		mvprintw(0,0,"Error: %s", msg.c_str());	
		refresh();
	}
	if ( !closeNormal ) {
		std::this_thread::sleep_for( timespan );
	}
	endwin();
	return 0;
}
