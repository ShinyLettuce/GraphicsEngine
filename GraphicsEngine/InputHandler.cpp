#include "InputHandler.h"
#include "CoolerWindows.h"

#define GET_X_PARAM(lParam) ((int)(short)((WORD)(((DWORD_PTR)(lParam)) & 0xffff)))
#define GET_Y_PARAM(lParam) ((int)(short)((WORD)((((DWORD_PTR)(lParam)) >> 16) & 0xffff)))

InputHandler::InputHandler() :
	myCurrentState{ 0 },
	myPreviousState{ 0 },
	myInputState{ 0 },
	myTentativeMouseDelta{0,0}

{
}

void InputHandler::InitHandle(HWND aWindowHandle)
{
	RAWINPUTDEVICE rawDevice;
	rawDevice.usUsagePage = { (USHORT)0x01 };
	rawDevice.usUsage = { (USHORT)0x02 };
	rawDevice.dwFlags = { RIDEV_INPUTSINK };
	rawDevice.hwndTarget = aWindowHandle;

	RegisterRawInputDevices(&rawDevice, 1, sizeof RAWINPUTDEVICE);


}

bool InputHandler::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
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
			myInputMousePosition.x = GET_X_PARAM(lParam);
			myInputMousePosition.y = GET_Y_PARAM(lParam);
			return true;;
		case WM_INPUT:
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				myTentativeMouseDelta.x += raw->data.mouse.lLastX;
				myTentativeMouseDelta.y += raw->data.mouse.lLastY;
			}
			return true;
	}
	return false;
}

bool InputHandler::IsKeyDown(const int aKeyCode) const
{
	return myCurrentState[aKeyCode];
}

bool InputHandler::IsKeyPressed(const int aKeyCode) const
{
	return myPreviousState[aKeyCode] ? false : myCurrentState[aKeyCode];
}

bool InputHandler::isKeyReleased(const int aKeyCode) const
{
	return myPreviousState[aKeyCode] ? !myCurrentState[aKeyCode] : false;
}

bool InputHandler::isButtonDown(const int aMouseButton) const
{
	return myCurrentState[aMouseButton];
}

POINT InputHandler::GetDeltaMousePosition() const
{
	return POINT{ myMouseDelta.x, myMouseDelta.y };
}

POINT InputHandler::GetMousePosition() const
{
	return myCurrentMousePosition;
}

void InputHandler::UpdateInput()
{
	myPreviousState = myCurrentState;
	myCurrentState = myInputState;

	//myPreviousMousePosition = myCurrentMousePosition;
	myCurrentMousePosition = myInputMousePosition;

	myMouseDelta = myTentativeMouseDelta;
	myTentativeMouseDelta = { 0,0 };

}
