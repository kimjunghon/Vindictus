#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CController abstract : public CBase
{
protected:	
	CController();
	virtual ~CController() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Update(_float fTimeDelta);

public:
	virtual HRESULT	MoveInput(INPUT_MOVE_DESC* pOut) PURE;
	virtual HRESULT	ActionInput(INPUT_ACTION_DESC* pOut) PURE;
	virtual HRESULT	CameraInput(INPUT_CAMERA_DESC* pOut) PURE;
	virtual HRESULT UI_Input(INPUT_UI_DESC* pOut) PURE;

protected:
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};


NS_END