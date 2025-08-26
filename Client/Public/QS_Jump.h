#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Jump final : public CMonsterState
{
private:
	CQS_Jump();
	virtual ~CQS_Jump() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_float2		m_vJumpReadyTime = {};
	_float2		m_vJumpMoveTime = {};

public:
	static CQS_Jump*	Create();
	virtual void		Free() override;
};

NS_END