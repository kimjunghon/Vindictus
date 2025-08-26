#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Idle final : public CMonsterState
{
private:
	CGS_Idle();
	virtual ~CGS_Idle() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CGS_Idle*	Create();
	virtual void		Free() override;
};

NS_END