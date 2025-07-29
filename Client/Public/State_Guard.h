#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)
class CState_Guard final : public CPlayerState
{
private:
	CState_Guard();
	virtual ~CState_Guard() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	_bool			m_bGuardEnd = {};
	_bool			m_bGuardBegin = {};
public:
	static CState_Guard*	Create();
	virtual void			Free() override;
};

NS_END