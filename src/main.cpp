#include <ncurses.h>

#include <iostream>
#include <chrono>
#include <thread>

#include "config.hpp"
#include "Game.hpp"

enum struct STATES { Input, Animation, Calculation, Exit };

int main(int argc, char *argv[])
{
	WINDOW* wnd = initscr();
	noecho();
	curs_set(FALSE);

	Game game{};
	using Clock = std::chrono::steady_clock;
	std::chrono::time_point<Clock> now;
	std::chrono::time_point<Clock> last;
	last = Clock::now();

	const std::chrono::duration<float> dt{FrameDuration};
	const std::chrono::duration<float> maxTime{MaxTimeFrame};

	std::chrono::duration<float> accumulator;
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
			clear();
			game.draw();
			refresh();
		}
	}
	std::chrono::milliseconds timespan(1000);
	std::this_thread::sleep_for(timespan);
	endwin();
	return 0;
}
