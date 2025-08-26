#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_NearAttack final : public CMonsterState
{
private:
	CQS_NearAttack();
	virtual ~CQS_NearAttack() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_float			m_fCheckDegree = {};

public:
	static CQS_NearAttack*	Create();
	virtual void			Free() override;
};

NS_END