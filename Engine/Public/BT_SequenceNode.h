#pragma once
#include "BT_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_SequenceNode final : public CBT_Node
{
private:
	CBT_SequenceNode();
	virtual ~CBT_SequenceNode() = default;

public:
	virtual BT_STATE	Evaluate() override;
	HRESULT				Add_Child(CBT_Node* pNode);
	
private:
	vector<CBT_Node*> m_Children;

public:
	static CBT_SequenceNode*	Create();
	virtual void				Free() override;
};

NS_END