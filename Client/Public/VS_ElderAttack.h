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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CVS_ElderAttack* Create();
	virtual void			Free() override;
};

NS_END