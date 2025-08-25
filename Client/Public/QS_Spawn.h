#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Spawn final : public CMonsterState
{
private:
	CQS_Spawn();
	virtual ~CQS_Spawn() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CQS_Spawn*	Create();
	virtual void		Free();
};

NS_END