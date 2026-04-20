#include "pch.h"
#include "InputHandler.h"
#include <windowsx.h>

CommonUtilities::InputHandler::InputHandler():
	myCurrentState{0},
	myPreviousState{0},
	myInputState{0}

{
}

bool CommonUtilities::InputHandler::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
			myInputState[wParam] = true;
			return true;
		case WM_KEYUP:
			myInputState[wParam] = false;
			return true;
		case WM_LBUTTONDOWN:
			myInputState[VK_LBUTTON] = true;
			return true;;
		case WM_LBUTTONUP:
			myInputState[VK_LBUTTON] = false;
			return true;;
		case WM_RBUTTONDOWN:
			myInputState[VK_RBUTTON] = true;
			return true;;
		case WM_RBUTTONUP:
			myInputState[VK_RBUTTON] = false;
			return true;;
		case WM_MBUTTONDOWN:
			myInputState[VK_MBUTTON] = true;
			return true;;
		case WM_MBUTTONUP:
			myInputState[VK_MBUTTON] = false;
			return true;;
		case WM_MOUSEMOVE:
			myInputMousePosition.x = GET_X_LPARAM(lParam);
			myInputMousePosition.y = GET_Y_LPARAM(lParam);
			return true;;
	}
	return false;
}

bool CommonUtilities::InputHandler::IsKeyDown(const int aKeyCode) const
{
	return myCurrentState[aKeyCode];
}

bool CommonUtilities::InputHandler::IsKeyPressed(const int aKeyCode) const
{
	return myPreviousState[aKeyCode] ? false : myCurrentState[aKeyCode];
}

bool CommonUtilities::InputHandler::isKeyReleased(const int aKeyCode) const
{
	return myPreviousState[aKeyCode] ? !myCurrentState[aKeyCode] : false;
}

bool CommonUtilities::InputHandler::isButtonDown(const int aMouseButton) const
{
	return myCurrentState[aMouseButton];
}

POINT CommonUtilities::InputHandler::GetDeltaMousePosition() const
{
	return POINT(myCurrentMousePosition.x - myPreviousMousePosition.x, myCurrentMousePosition.y - myPreviousMousePosition.y);
}

POINT CommonUtilities::InputHandler::GetMousePosition() const
{
	return myCurrentMousePosition;
}

void CommonUtilities::InputHandler::UpdateInput()
{
	myPreviousState = myCurrentState;
	myCurrentState = myInputState;

	myPreviousMousePosition = myCurrentMousePosition;
	myCurrentMousePosition = myInputMousePosition;

}
