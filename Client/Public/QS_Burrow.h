#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Queen;

class CQS_Burrow final : public CMonsterState
{
private:
	enum class BURROW_STATE { BEGIN, DURING, END};
	enum class BURROW_ATTACK { MOVE, ATTACK1, ATTACK2, END };

private:
	CQS_Burrow();
	virtual ~CQS_Burrow() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CMonster* pMonster) override;
	virtual void	Update(CMonster* pMonster, _float fTimeDelta) override;
	virtual void	Exit(CMonster* pMonster) override;


private:
	_bool			m_IsBurrowAction[ENUM_CLASS(BURROW_ATTACK::END)] = {};
	BURROW_STATE	m_eBurrowState = {};

private:
	void			Update_During(CMonster* pMonster, _float fTimeDelta);
	void			Attack(CMonster* pMonster, _float fTimeDelta);
	void			Move(CMonster* pMonster, _float fTimeDelta);
public:
	static CQS_Burrow*	Create();
	virtual void		Free() override;
};

NS_END