#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGS_Hang final : public CMonsterState
{
private:
	enum class HANG_PHASE { BEGIN, DURING, END };

private:
	CGS_Hang();
	virtual ~CGS_Hang() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_bool		m_IsChange = {};
	_uint		m_iHangFlag = {};
	_uint		m_iHangPhase = {};
	_uint		m_iAttackCount = {};
	_uint		m_iCurrentAttackCount = {};


	_float2		m_vMoveReadyTime = {};
	_float2		m_vMoveTime = {};

public:
	static CGS_Hang*	Create();
	virtual void		Free() override;
};

NS_END