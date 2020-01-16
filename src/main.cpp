
#include <iostream>
#include <chrono>
#include <thread>

#include "config.hpp"
#include "Game.hpp"
#include "Msg.hpp"

enum struct STATES { Input, Animation, Calculation, Exit };
std::unique_ptr<Glob> Glob::m_instance{nullptr};

int main(int argc, char *argv[])
{
	WINDOW* wnd = initscr();

	if (has_colors() == FALSE) {
		std::cerr << "You're terminal don't support colors, sorry\n";
	}
	start_color();

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
				// clear();
				game.draw();
				msgQueue.fetchEvents( wnd );
				while(msgQueue.pop(msg)) {	
					game.input(msg);
				}
			}

		}
	} catch ( const std::string& msg) {
		closeNormal = false;
		mvprintw(0,0,"Error: %s", msg.c_str());	
		refresh();
	}
	if ( !closeNormal ) {
		std::chrono::milliseconds timespan( 1000 );
		std::this_thread::sleep_for( timespan );
	}
	endwin();
	return 0;
}
