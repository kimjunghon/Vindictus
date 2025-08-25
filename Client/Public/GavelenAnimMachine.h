#pragma once
#include "AnimMachine.h"

using namespace Glasgavelen;

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
	void			Initialize_RAGE();
	void			Initialize_GRAP();
	void			Initialize_HANG();
	void			Initialize_HIT();
	void			Initialize_Dead();

public:
	static CGavelenAnimMachine*		Create();
	virtual void					Free() override;
};

NS_END