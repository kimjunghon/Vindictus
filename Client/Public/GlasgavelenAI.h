#pragma once
#include "Client_Defines.h"
#include "BehaviorTree.h"

NS_BEGIN(Client)

class CGlasgavelen;

class CGlasgavelenAI final : public CBehaviorTree
{
private:
	CGlasgavelenAI();
	virtual ~CGlasgavelenAI() = default;

public:
	virtual HRESULT Initialize(CGlasgavelen* pControlledGlasgavelen);

private:
	CGlasgavelen* m_pControlledGlasgavelen = { nullptr };

private:
	HRESULT Ready_Nodes();

public:
	static CGlasgavelenAI* Create(CGlasgavelen* pControlledGlasgavelen);
	virtual void Free() override;
};

NS_END