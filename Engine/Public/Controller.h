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

protected:
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};


NS_END