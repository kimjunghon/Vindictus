#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_Idle final : public CMonsterState
{
private:
	CVS_Idle();
	virtual ~CVS_Idle() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CVS_Idle*	Create();
	virtual void		Free();
};

NS_END