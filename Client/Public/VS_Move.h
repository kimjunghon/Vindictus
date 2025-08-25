#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_Move final : public CMonsterState
{
private:
	CVS_Move();
	virtual ~CVS_Move() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CVS_Move*	Create();
	virtual void		Free();
};

NS_END