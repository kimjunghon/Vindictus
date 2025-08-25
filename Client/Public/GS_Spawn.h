#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Spawn final : public CMonsterState
{
private:
	CGS_Spawn();
	virtual ~CGS_Spawn() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

public:
	static CGS_Spawn*	Create();
	virtual void		Free();
};

NS_END