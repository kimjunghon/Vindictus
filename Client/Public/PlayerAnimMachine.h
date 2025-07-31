#pragma once
#include "AnimMachine.h"

using namespace Player;

NS_BEGIN(Client)

class CPlayerAnimMachine final : public CAnimMachine
{
private:
	CPlayerAnimMachine();
	virtual ~CPlayerAnimMachine() = default;

public:
	virtual void	Initialize();
	void			Initialize_IDLE();
	void			Initialize_MOVE();
	void			Initialize_ATTACK();
	void			Initialize_SMASH();
	void			Initialize_GUARD();
	void			Initialize_HEAVYSTAND();
	void			Initialize_ROLL();
	void			Initialize_HIT();

public:
	static CPlayerAnimMachine*	Create();
	virtual void				Free() override;
};

NS_END