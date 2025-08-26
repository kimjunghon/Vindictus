#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

class CGlasgavelen;

using namespace Glasgavelen;

class CGS_Grap final : public CMonsterState
{
private:
	enum class GRAP_PHASE { BEGIN, END };

private:
	CGS_Grap();
	virtual ~CGS_Grap() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_bool		m_IsChange = {};
	GRAP_PHASE	m_eGrapPhase = {};

public:
	static CGS_Grap*	Create();
	virtual void		Free() override;
};

NS_END