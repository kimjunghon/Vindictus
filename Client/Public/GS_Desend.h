#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Desend final : public CMonsterState
{
private:
	CGS_Desend();
	virtual ~CGS_Desend() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_float2			m_vMoveReadyTime = {};
	_float2			m_vMoveTime = {};

public:
	static CGS_Desend*		Create();
	virtual void			Free() override;
};

NS_END