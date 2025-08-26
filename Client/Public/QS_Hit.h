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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_float			m_fCheckDegree = {};

public:
	static CQS_Hit* Create();
	virtual void	Free() override;
};

NS_END