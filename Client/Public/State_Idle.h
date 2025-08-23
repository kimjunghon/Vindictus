#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Idle final : public CPlayerState
{
private:
	CState_Idle();
	virtual ~CState_Idle() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	_float			m_fIdleTime = {};
	_float			m_fRestDelay = {};

public:
	static CState_Idle* Create();
	virtual void		Free() override;
};

NS_END