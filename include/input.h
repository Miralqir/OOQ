#pragma once

class InputHandler
{
private:
	bool quit;
	bool up;
	bool right;
	bool down;
	bool left;
	bool up2;
	bool right2;
	bool down2;
	bool left2;

public:
	void processEvents();
	bool isQuit();
	bool isUp();
	bool isRight();
	bool isDown();
	bool isLeft();
	bool isUp2();
	bool isRight2();
	bool isDown2();
	bool isLeft2();
};
