#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_RageAttack final : public CMonsterState
{
private:
	CGS_RageAttack();
	virtual ~CGS_RageAttack() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_uint			m_iLinkAnimAttackFlag = {};

private:
	void			ChangeAttackState(CMonster* pMonster, _uint iAttackFlag);

public:
	static CGS_RageAttack*	Create();
	virtual void			Free() override;
};

NS_END