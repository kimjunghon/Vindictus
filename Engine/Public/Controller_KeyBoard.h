#pragma once
#include "Controller.h"

NS_BEGIN(Engine)

class ENGINE_DLL CController_KeyBoard final : public CController
{
private:
	CController_KeyBoard();
	virtual ~CController_KeyBoard() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT	MoveInput(INPUT_MOVE_DESC* pOut) override;
	virtual HRESULT	ActionInput(INPUT_ACTION_DESC* pOut) override;
	virtual HRESULT	CameraInput(INPUT_CAMERA_DESC* pOut) override;

public:
	static CController_KeyBoard* Create();
	virtual void Free() override;
};

NS_END