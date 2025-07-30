#pragma once
#include <windows.h>

class macro {
public:
	void MoveMouseRelative(int dx, int dy);
	bool keyDown(int keyCode);
};

void macro::MoveMouseRelative(int dx, int dy) {
	mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
}

bool macro::keyDown(int keyCode) {
	return GetAsyncKeyState(keyCode) & 0x8000;
}