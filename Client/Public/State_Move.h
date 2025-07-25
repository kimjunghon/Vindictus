#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Move final : public CPlayerState
{
private:
	CState_Move();
	virtual ~CState_Move() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

private:
	_bool			m_bStop;

public:
	static CState_Move* Create();
	virtual void		Free() override;
};

NS_END