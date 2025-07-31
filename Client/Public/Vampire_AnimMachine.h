#pragma once
#include "AnimMachine.h"

using namespace Vampire;

NS_BEGIN(Client)

class CVampire_AnimMachine final : public CAnimMachine
{
private:
	CVampire_AnimMachine();
	virtual ~CVampire_AnimMachine() = default;

public:
	virtual void	Initialize();
	void			Initialize_SPAWN();
	void			Initialize_IDLE();
	void			Initialize_MOVE();
	void			Initialize_ATTACK();
	void			Initialize_HIT();

public:
	static CVampire_AnimMachine*	Create();
	virtual void					Free() override;
};

NS_END