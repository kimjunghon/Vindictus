#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CEvent abstract : public CBase
{
protected:
		virtual ~CEvent() = default;
};

NS_END