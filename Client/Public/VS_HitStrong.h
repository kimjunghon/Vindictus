#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)

using namespace Vampire;

class CVS_HitStrong final : public CMonsterState
{
private:
	enum class STRONG_PHASE { BEGIN, DURING, END, UP};

private:
	CVS_HitStrong();
	virtual ~CVS_HitStrong() = default;

public:
	virtual _bool	CanStateChange(CMonster* pMonster) override;

public:
	virtual HRESULT Initialize();
	virtual void	Enter(CMonster* pMonster);
	virtual void	Update(CMonster* pMonster, _float fTimeDelta);
	virtual void	Exit(CMonster* pMonster);

private:
	_uint			m_iStrongFlag = {};
	_uint			m_iAnimPhase = {};

public:
	static CVS_HitStrong*	Create();
	virtual void			Free();
};

NS_END