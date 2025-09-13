#pragma once
#include "AnimMachine.h"

using namespace Puppy;

NS_BEGIN(Client)

class CPuppyAnimMachine final : public CAnimMachine
{
private:
	CPuppyAnimMachine();
	virtual ~CPuppyAnimMachine() = default;

public:
	virtual void	Initialize() override;

public:
	static CPuppyAnimMachine*	Create();
	virtual void				Free() override;
};

NS_END