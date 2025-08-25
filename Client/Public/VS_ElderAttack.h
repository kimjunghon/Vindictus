#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_ElderAttack final : public CMonsterState
{
private:
	CVS_ElderAttack();
	virtual ~CVS_ElderAttack() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CVS_ElderAttack* Create();
	virtual void		Free();
};

NS_END