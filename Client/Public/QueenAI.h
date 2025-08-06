#pragma once
#include "Client_Defines.h"
#include "BehaviorTree.h"

NS_BEGIN(Client)

class CQueen;

class CQueenAI final : public CBehaviorTree
{
private:
	CQueenAI();
	virtual ~CQueenAI() = default;

public:
	virtual HRESULT Initialize(CQueen* pControlledQueen);

private:
	CQueen* m_pControlledQueen = { nullptr };

private:
	HRESULT Ready_Nodes();

public:
	static CQueenAI* Create(CQueen* pControlledQueen);
	virtual void Free() override;
};

NS_END