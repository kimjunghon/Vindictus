#pragma once
#include "BT_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_SelectorNode final : public CBT_Node
{
private:
	CBT_SelectorNode();
	virtual ~CBT_SelectorNode() = default;

public:
	virtual BT_STATE	Evaluate() override;
	HRESULT				Add_Child(CBT_Node* pNode);

private:
	vector<CBT_Node*> m_Children;

public:
	static CBT_SelectorNode* Create();
	virtual void Free();
};

NS_END