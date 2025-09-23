#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Cutscene final : public CMonsterState
{
private:
	CGS_Cutscene();
	virtual ~CGS_Cutscene() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

public:
	static CGS_Cutscene* Create();
	virtual void		Free() override;
};

NS_END