#pragma once

#include <any>
#include <queue>
#include <optional>
#include <typeindex>
#include <fstream>

#include "config.hpp"

enum struct MsgTypes { Key, LeftClick };

class Msg {
public:
	static std::optional<Msg> read(WINDOW* wnd) {
		int res = wgetch(wnd);
		if(res != ERR) {
			if( res == KEY_MOUSE ) {
				MEVENT event;
				if(getmouse(&event) == OK) {
					switch (event.bstate) {
						case BUTTON1_CLICKED:
							return std::optional<Msg>(Msg(
									MsgTypes::LeftClick, 
									Pos(event.x, event.y)));
					}
				}
			} else {
				return std::optional<Msg>(Msg(MsgTypes::Key, res));
			}
		}
		return std::nullopt;
	}
	template<typename T>
	const T* fetch(MsgTypes type) const {
		if (type == m_msgType) {
			if (std::type_index(m_value.type()) != std::type_index(typeid(T))) {
				throw std::string("type mismatch");
			}
			return std::any_cast<const T*>(m_value);
		}
		return nullptr;
	} 

	Msg& operator=(const Msg&) = default;
	Msg(const Msg&) = default;
	Msg& operator=(Msg&&) = default;
	Msg(Msg&&) = default;
	Msg() = default;
private:
	template<typename T>
	Msg(MsgTypes type, T value) : m_msgType(type), m_value(value){}
	std::any m_value;
	MsgTypes m_msgType;
};

class MsgQueue {
public:
	bool pop(Msg& msg) {
		if(m_msgs.empty()) {return false;}
		msg = m_msgs.front();
		m_msgs.pop();
		return true;
	}
	void fetchEvents(WINDOW* wnd) {
		std::optional<Msg> msg = std::nullopt;
		wgetch(wnd);
		/*while((msg = Msg::read(wnd))) {
			m_msgs.push(msg.value());
		}*/
		
	}
private:
	std::queue<Msg> m_msgs;
};
