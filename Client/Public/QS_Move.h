#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Move final : public CMonsterState
{
private:
	CQS_Move();
	virtual ~CQS_Move() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CQS_Move*	Create();
	virtual void		Free() override;
};

NS_END