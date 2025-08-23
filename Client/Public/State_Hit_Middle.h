#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Hit_Middle final : public CPlayerState
{
private:
	CState_Hit_Middle();
	virtual ~CState_Hit_Middle() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	_uint			m_iDirFlag = {};

private:
	void			Change_HitFlag(HIT_DIR eHitDir);

public:
	static CState_Hit_Middle*	Create();
	virtual void				Free() override;
};

NS_END