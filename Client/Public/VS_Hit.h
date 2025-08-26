#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_Hit final : public CMonsterState
{
private:
	CVS_Hit();
	virtual ~CVS_Hit() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_float			m_fCheckDegree = {};

public:
	static CVS_Hit* Create();
	virtual void	Free() override;
};

NS_END