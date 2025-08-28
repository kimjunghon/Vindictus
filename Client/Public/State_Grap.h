#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)

class CState_Grap final : public CPlayerState
{
private:
	CState_Grap();
	virtual ~CState_Grap() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Enter(CPlayerPawn* pPlayerPawn) override;
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput) override;
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn) override;

public:
	static CState_Grap*			Create();
	virtual void				Free() override;
};

NS_END