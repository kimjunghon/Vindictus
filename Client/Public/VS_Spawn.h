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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CVS_Spawn*	Create();
	virtual void		Free() override;
};

NS_END