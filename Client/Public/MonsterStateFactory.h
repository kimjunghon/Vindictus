#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CMonsterState;

class CMonsterStateFactory final : public CBase
{
	DECLARE_SINGLETON(CMonsterStateFactory);

private:
	typedef function<CMonsterState* ()> STATECREATE;
	typedef unordered_map<pair<_uint, _uint>, STATECREATE, PairHash<_uint>> CREATEMAP;

private:
	CMonsterStateFactory();
	virtual ~CMonsterStateFactory() = default;

public:
	void			Register(const _uint iType, const _uint iState, STATECREATE Func) { m_CreateMap[make_pair(iType, iState)] = Func; }
	CMonsterState*	Create(const _uint iType, const _uint iState) { return m_CreateMap[make_pair(iType, iState)](); }

private:
	STATECREATE							m_CreateFunc;
	CREATEMAP							m_CreateMap;

public:
	virtual void Free() override;
};

NS_END