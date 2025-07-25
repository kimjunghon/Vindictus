#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CPlayerPawn;
class CStateFactory;

class CPlayerState abstract : public CBase
{
protected:
	CPlayerState();
	virtual ~CPlayerState() = default;

public:
	void Bind_StateFlag(_uint& iStateFlag) { iStateFlag = m_iStateFlag; }
public:
	virtual HRESULT Initialize();
	virtual void	Enter(CPlayerPawn* pPlayerPawn);
	virtual void	InputData(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput);
	virtual void	Update(CPlayerPawn* pPlayerPawn, _float fTimeDelta);
	virtual void	Exit(CPlayerPawn* pPlayerPawn);

protected:
	_uint m_iStateFlag = {};

protected:
	void			Change_OtherState(CPlayerPawn* pPlayerPawn, INPUT_MOVE_DESC MoveInput, INPUT_ACTION_DESC ActionInput);
	void			Find_ActionState(CPlayerPawn* pPlayerPawn, _byte byAction);
	void			ChangeActionFlag(_uint iOnFlag);
public:
	virtual void			Free() override;
};

NS_END