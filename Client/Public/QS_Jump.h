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
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

private:
	_float2		m_vJumpReadyTime = {};
	_float2		m_vJumpMoveTime = {};

public:
	static CQS_Jump* Create();
	virtual void		Free();
};

NS_END