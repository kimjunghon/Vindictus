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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CQS_Idle*	Create();
	virtual void		Free() override;
};

NS_END