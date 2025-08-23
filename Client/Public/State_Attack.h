#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Attack final : public CPlayerState
{
private:
	CState_Attack();
	virtual ~CState_Attack() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	_bool			m_bReadyAttack = {};

	_float			m_fKeepTime = {};
	_float			m_fCurrentKeepTime = {};
public:
	static CState_Attack*	Create();
	virtual void			Free() override;
};

NS_END