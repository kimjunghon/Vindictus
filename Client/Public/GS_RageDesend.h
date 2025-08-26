#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_RageDesend final : public CMonsterState
{
private:
	CGS_RageDesend();
	virtual ~CGS_RageDesend() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_bool			m_IsChange = {};
	_float2			m_vMoveReadyTime = {};
	_float2			m_vMoveTime = {};

private:
	void			Update_Begin(CMonster* pMonster, _float fTimeDelta);
	void			Update_End(CMonster* pMonster, _float fTimeDelta);

public:
	static CGS_RageDesend*	Create();
	virtual void			Free() override;
};

NS_END