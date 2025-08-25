#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Hit final : public CMonsterState
{
private:
	CQS_Hit();
	virtual ~CQS_Hit() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

private:
	_float			m_fCheckDegree = {};

public:
	static CQS_Hit* Create();
	virtual void	Free();
};

NS_END