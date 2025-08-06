#pragma once
#include "Client_Defines.h"
#include "Controller.h"

NS_BEGIN(Client)

class CController_UI final : public CController
{
private:
	CController_UI();
	virtual ~CController_UI() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT UI_Input(INPUT_UI_DESC* pOut) override;

public:
	static CController_UI*	Create();
	virtual void			Free() override;

};

NS_END