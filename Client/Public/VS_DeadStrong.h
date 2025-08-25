#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_DeadStrong final : public CMonsterState
{
private:
	CVS_DeadStrong();
	virtual ~CVS_DeadStrong() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CVS_DeadStrong* Create();
	virtual void		Free();
};

NS_END