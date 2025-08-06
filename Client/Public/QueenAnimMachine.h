#pragma once
#include "Client_Defines.h"
#include "AnimMachine.h"

using namespace Queen;

NS_BEGIN(Client)

class CQueenAnimMachine final : public CAnimMachine
{
private:
	CQueenAnimMachine();
	virtual ~CQueenAnimMachine() = default;

public:
	virtual void	Initialize();
	void			Initialize_CUTSEAN();
	void			Initialize_IDLE();
	void			Initialize_MOVE();
	void			Initialize_ATTACK();
	void			Initialize_BURROW();
	void			Initialize_HIT();

public:
	static CQueenAnimMachine*	Create();
	virtual void				Free() override;
};

NS_END