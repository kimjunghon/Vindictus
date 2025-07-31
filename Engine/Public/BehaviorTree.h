#pragma once
#include "Base.h"

#include "BT_SelectorNode.h"
#include "BT_ActionNode.h"
#include "BT_SequenceNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBehaviorTree abstract : public CBase
{
protected:
	CBehaviorTree();
	virtual ~CBehaviorTree() = default;

public:
	virtual HRESULT Initialize();
	void			Update();

protected:
	CBT_Node* m_pRootNode;
	
public:
	virtual void Free() override;
};

NS_END