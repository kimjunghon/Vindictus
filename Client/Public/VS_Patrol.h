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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CVS_Patrol*	Create();
	virtual void		Free() override;
};

NS_END