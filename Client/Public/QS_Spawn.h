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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CQS_Spawn*	Create();
	virtual void		Free() override;
};

NS_END