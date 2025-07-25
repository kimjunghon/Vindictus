#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Smash0 final : public CPlayerState
{
private:
	CState_Smash0();
	virtual ~CState_Smash0() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	_float			m_fFullChargeTime = {};
	_float			m_fChargeTime = {};
	_bool			m_bCharging = {};
	_bool			m_bSmashEnd = {};

public:
	static CState_Smash0*	Create();
	virtual void			Free() override;
};

NS_END