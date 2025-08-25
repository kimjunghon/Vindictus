#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Attack final : public CMonsterState
{
private:
	CQS_Attack();
	virtual ~CQS_Attack() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CQS_Attack*	Create();
	virtual void		Free();
};

NS_END