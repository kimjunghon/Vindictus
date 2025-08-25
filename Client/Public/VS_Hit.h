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
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

private:
	_float			m_fCheckDegree = {};

public:
	static CVS_Hit* Create();
	virtual void	Free();
};

NS_END