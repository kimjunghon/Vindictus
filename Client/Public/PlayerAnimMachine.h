#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CPlayerAnimMachine final : public CBase
{
private:
	CPlayerAnimMachine();
	virtual ~CPlayerAnimMachine() = default;


public:
	void	Initialize();
	void	Initialize_IDLE();
	void	Initialize_MOVE();
	void	Initialize_ATTACK();
	void	Initialize_SMASH();
	void	Initialize_GUARD();
	void	Initialize_HEAVYSTAND();
	void	Initialize_ROLL();
	void	Initialize_HIT();

	HRESULT Set_Animation(CModel* pModelCom, _uint iState);

private:
	unordered_map<_uint, ANIM_DATA> m_AnimDatas;

public:
	static CPlayerAnimMachine*	Create();
	virtual void				Free() override;
};

NS_END