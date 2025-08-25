#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_Patrol final : public CMonsterState
{
private:
	CVS_Patrol();
	virtual ~CVS_Patrol() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CVS_Patrol* Create();
	virtual void			Free();
};

NS_END