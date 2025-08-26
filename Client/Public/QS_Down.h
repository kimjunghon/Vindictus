#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Down final : public CMonsterState
{
private:
	enum class DOWN_PHASE { BEGIN, DURING, END};

private:
	CQS_Down();
	virtual ~CQS_Down() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_uint			m_iStrongFlag = {};
	_uint			m_iAnimPhase = {};

public:
	static CQS_Down*	Create();
	virtual void		Free() override;
};

NS_END