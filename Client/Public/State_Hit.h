#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Hit final : public CPlayerState
{
private:
	CState_Hit();
	virtual ~CState_Hit() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	void			Change_HitFlag(HIT_TYPE eHitType);

public:
	static CState_Hit* Create();
	virtual void		Free() override;
};

NS_END