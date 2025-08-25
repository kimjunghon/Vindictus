#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Attack final : public CMonsterState
{
private:
	CGS_Attack();
	virtual ~CGS_Attack() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CGS_Attack*	Create();
	virtual void		Free();
};

NS_END