#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Idle final : public CMonsterState
{
private:
	CQS_Idle();
	virtual ~CQS_Idle() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CQS_Idle*	Create();
	virtual void		Free();
};

NS_END