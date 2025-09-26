#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Dead final : public CMonsterState
{
private:
	CGS_Dead();
	virtual ~CGS_Dead() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CGS_Dead* Create();
	virtual void		Free() override;
};

NS_END