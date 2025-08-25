#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CState abstract : public CBase
{
protected:
	CState();
	virtual ~CState() = default;

public:
	void			Bind_StateFlag(_uint& iStateFlag) { iStateFlag = m_iStateFlag; }

public:
	virtual HRESULT Initialize();

protected:
	_uint			m_iStateFlag = {};

public:
	virtual void	Free() override;
};

NS_END