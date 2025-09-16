#pragma once
#include "AnimMachine.h"

using namespace Cat;

NS_BEGIN(Client)

class CCatAnimMachine final : public CAnimMachine
{
private:
	CCatAnimMachine();
	virtual ~CCatAnimMachine() = default;

public:
	virtual void	Initialize() override;

public:
	static CCatAnimMachine*		Create();
	virtual void				Free() override;
};

NS_END