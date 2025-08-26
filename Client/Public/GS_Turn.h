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
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_uint			GetTurnFlag(DIR eRightDir);

public:
	static CGS_Turn*	Create();
	virtual void		Free() override;
};

NS_END