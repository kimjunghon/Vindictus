#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CController abstract : public CBase
{
protected:	
	CController();
	virtual ~CController() = default;

public:
	_byte		Get_MoveInputData() { return m_byMoveInputData; }
	_short		Get_ActionInputData() { return m_iActionInputData; }
	_vector		Get_CameraInputData() { return XMLoadFloat3(&m_vCameraInputData); }

public:
	virtual HRESULT Initialize();
	void			Update_Controller();
	void			Clear_Data();

protected:
	virtual void MoveInput() PURE;
	virtual void ActionInput() PURE;
	virtual void CameraInput() PURE;

protected:
	_byte					m_byMoveInputData = {};
	_short					m_iActionInputData = {};
	_float3					m_vCameraInputData = {};

	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};


NS_END