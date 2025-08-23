#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)
class CState_Roll final : public CPlayerState
{
private:
	CState_Roll();
	virtual ~CState_Roll() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

public:
	ANIM_PHASE		m_eAnimPhase = {};

public:
	static CState_Roll* Create();
	virtual void		Free() override;
};

NS_END