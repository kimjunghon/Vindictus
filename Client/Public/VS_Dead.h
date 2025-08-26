#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_Dead final : public CMonsterState
{
private:
	CVS_Dead();
	virtual ~CVS_Dead() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CVS_Dead*	Create();
	virtual void		Free() override;
};

NS_END