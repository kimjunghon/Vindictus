#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Turn final : public CMonsterState
{
private:
	CQS_Turn();
	virtual ~CQS_Turn() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;

private:
	_float			m_fBackDegree = {};

private:
	_uint			GetTurnFlag(DIR eRightDir, DIR eBackDir);

public:
	static CQS_Turn*	Create();
	virtual void		Free() override;
};

NS_END