#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Hit_HeavyStand final : public CPlayerState
{
private:
	CState_Hit_HeavyStand();
	virtual ~CState_Hit_HeavyStand() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	_float			m_fHitTime = {};
	_float			m_fCanCounterTime = {};

public:
	static CState_Hit_HeavyStand*	Create();
	virtual void					Free() override;
};

NS_END