#pragma once
#include <Windows.h>
#include <array>

class InputHandler
{
public:

	InputHandler();

	/// <summary>
	/// Should be called as part of Windows' message handling.
	/// Registers all input messages to be handled this frame.
	/// </summary>
	bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);

	bool IsKeyDown(const int aKeyCode) const;
	bool IsKeyPressed(const int aKeyCode) const;
	bool isKeyReleased(const int aKeyCode) const;
	bool isButtonDown(const int aMouseButton) const;
	POINT GetMousePosition() const;
	POINT GetDeltaMousePosition() const;

	/// <summary>
	/// Should be called after Windows' message handling.
	/// Updates all input states to be queried for this frame.
	/// </summary>
	void UpdateInput();

private:
	std::array<bool, 256> myCurrentState;
	std::array<bool, 256> myPreviousState;
	std::array<bool, 256> myInputState;

	POINT myPreviousMousePosition;
	POINT myCurrentMousePosition;
	POINT myInputMousePosition;
};

