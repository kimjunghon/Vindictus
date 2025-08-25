#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Turn final : public CMonsterState
{
private:
	CGS_Turn();
	virtual ~CGS_Turn() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

private:
	_uint			GetTurnFlag(DIR eRightDir);

public:
	static CGS_Turn*	Create();
	virtual void		Free();
};

NS_END