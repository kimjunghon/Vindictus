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
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

private:
	_float			m_fCheckDegree = {};

public:
	static CQS_NearAttack*	Create();
	virtual void			Free();
};

NS_END