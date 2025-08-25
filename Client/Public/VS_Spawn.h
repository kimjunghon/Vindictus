#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_Spawn final : public CMonsterState
{
private:
	CVS_Spawn();
	virtual ~CVS_Spawn() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CVS_Spawn*	Create();
	virtual void		Free();
};

NS_END