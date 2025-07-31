#pragma once
#include "BT_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_ActionNode final : public CBT_Node
{
private:
	CBT_ActionNode(function<BT_STATE()> Action);
	virtual ~CBT_ActionNode() = default;

public:
	virtual BT_STATE		Evaluate() override;

private:
	function<BT_STATE()> m_Action;
	
public:
	static CBT_ActionNode*	Create(function<BT_STATE()> Action);
	virtual void			Free() override;
};

NS_END