#include <ncurses.h>

#include <iostream>
#include <chrono>
#include <thread>

#include "Board.hpp"

enum struct STATES { room, exit };

int main(int argc, char *argv[])
{
	initscr();
	noecho();
	curs_set(FALSE);

	STATES state = STATES::room;

	while (state != STATES::exit) {
		clear();
		switch (state) {
			case STATES::room:
	
				break;
			default:
					std::cout << "EXIT\n";
				break;
		}
		refresh();
	}

	std::chrono::milliseconds timespan(1000);
	std::this_thread::sleep_for(timespan);
	endwin();
	return 0;
}
