#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_Node abstract : public CBase
{
protected:
	CBT_Node();
	virtual ~CBT_Node() = default;

public:
	virtual BT_STATE	Evaluate() PURE;

public:
	virtual void		Free() override;
};

NS_END