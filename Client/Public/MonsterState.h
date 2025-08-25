#pragma once
#include "State.h"
#include "MonsterStateFactory.h"

NS_BEGIN(Client)

class CMonster;

class CMonsterState abstract : public CState
{
protected:
	CMonsterState();
	virtual ~CMonsterState() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster);

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	virtual void	Free() override;
};

NS_END