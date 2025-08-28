#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGlasgavelen;

class CGS_Down final : public CMonsterState
{
private:
	enum class DOWN_PHASE { BEGIN, DURING, END };

private:
	CGS_Down();
	virtual ~CGS_Down() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_uint			m_iDownPhase = {};
	_uint			m_iDownFlag = {};

public:
	static CGS_Down*	Create();
	virtual void		Free() override;
};

NS_END