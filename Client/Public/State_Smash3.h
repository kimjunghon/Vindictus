#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Smash3 final : public CPlayerState
{
private:
	CState_Smash3();
	virtual ~CState_Smash3() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

public:
	_uint		m_iActionFlag = {};
	_uint		m_iComboCount = {};
	_uint		m_iMaxComboCount = {};

	_bool		m_bReadySmash = {};
	_float		m_fKeepTime = {};
	_float		m_fCurrentKeepTime = {};

public:
	static CState_Smash3*	Create();
	virtual void			Free() override;
};

NS_END