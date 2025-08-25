#pragma once
#include "AnimMachine.h"

using namespace Glas;

NS_BEGIN(Client)

class CGavelenAnimMachine final : public CAnimMachine
{
private:
	CGavelenAnimMachine();
	virtual ~CGavelenAnimMachine() = default;

public:
	virtual void	Initialize();
	void			Initialize_SPAWN();
	void			Initialize_IDLE();
	void			Initialize_MOVE();
	void			Initialize_ATTACK();
	void			Initialize_HIT();
	void			Initialize_Dead();

public:
	static CGavelenAnimMachine* Create();
	virtual void					Free() override;
};

NS_END