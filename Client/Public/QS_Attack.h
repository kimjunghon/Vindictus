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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CQS_Attack*	Create();
	virtual void		Free() override;
};

NS_END