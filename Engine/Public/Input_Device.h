#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CInput_Device final : public CBase
{
private:
	CInput_Device();
	virtual ~CInput_Device() = default;


public:
	_byte	Get_DIKeyState(_ubyte byKeyID) {
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) {
		return m_MouseState.rgbButtons[ENUM_CLASS(eMouse)];
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState) {
		/*switch (eMouseState)
		{
		case X:
			return m_tMouseState.lX;
		case Y:
			return m_tMouseState.lY;
		case Z:
			return m_tMouseState.lZ;
		}*/
		return *((reinterpret_cast<_int*>(&m_MouseState)) + ENUM_CLASS(eMouseState));
	}

	_bool	Get_KeyDown(_ubyte byKeyID) {
		return (m_byKeyState[byKeyID] & 0x80) && (~m_byOldKeyState[byKeyID] & 0x80);
	}

	_bool	Get_KeyUp(_ubyte byKeyID) {
		return (m_byOldKeyState[byKeyID] & 0x80) && (~m_byKeyState[byKeyID] & 0x80);
	}

	_bool	Get_KeyPressing(_ubyte byKeyID) {
		return (m_byKeyState[byKeyID] & 0x80) && (m_byOldKeyState[byKeyID] & 0x80);
	}

public:
	HRESULT		Initialize(HINSTANCE hInstance, HWND hWnd);
	void		Update();

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

	_byte					m_byOldKeyState[256] = {};
	_byte					m_byKeyState[256] = {};
	DIMOUSESTATE			m_MouseState = {};

public:
	static CInput_Device*	Create(HINSTANCE hInstance, HWND hWnd);
	virtual void			Free() override;
};

NS_END