#pragma once
#include "Client_Defines.h"
#include "BehaviorTree.h"

NS_BEGIN(Client)

class CVampire;

class CVampireAI final : public CBehaviorTree
{
private:
	CVampireAI();
	virtual ~CVampireAI() = default;

public:
	virtual HRESULT Initialize(CVampire* pControlledVampire);

private:	
	CVampire* m_pControlledVampire = { nullptr };

private:
	HRESULT Ready_Nodes();

public:
	static CVampireAI* Create(CVampire* pControlledVampire);
	virtual void Free() override;
};

NS_END