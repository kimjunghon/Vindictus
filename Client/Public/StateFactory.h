#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CPlayerState;

class CStateFactory final : public CBase
{
	DECLARE_SINGLETON(CStateFactory);

private:
	typedef function<CPlayerState*()> STATECREATE;

private:
	CStateFactory();
	virtual ~CStateFactory() = default;
	
public:
	void			Register(const _uint iState, STATECREATE Func) { m_CreateMap[iState] = Func; }
	CPlayerState*	Create(const _uint iState) { return m_CreateMap[iState](); }

private:
	STATECREATE m_CreateFunc;
	unordered_map<_uint, STATECREATE> m_CreateMap;

public:
	virtual void Free() override;
};

NS_END