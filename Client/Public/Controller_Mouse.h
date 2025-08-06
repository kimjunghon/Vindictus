#pragma once
#include "Client_Defines.h"
#include "Controller.h"

NS_BEGIN(Client)

class CController_Mouse final : public CController
{
private:
	CController_Mouse();
	virtual ~CController_Mouse() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;

public:
	virtual HRESULT	MoveInput(INPUT_MOVE_DESC* pOut) override;
	virtual HRESULT	ActionInput(INPUT_ACTION_DESC* pOut) override;
	virtual HRESULT	CameraInput(INPUT_CAMERA_DESC* pOut) override;

private:
	_float		m_fSensor = {};

public:
	static CController_Mouse*	Create();
	virtual void				Free() override;
};

NS_END