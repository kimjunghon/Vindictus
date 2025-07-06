#pragma once
#include "Base.h"

NS_BEGIN(Engine)

template<typename TObject, typename TVolume>
class COctreeTest final : public CBase
{
private:
	struct OCTREE_NODE : public NODE<TObject, TVolume>
	{
		vector<TObject*>		Objects;
		OCTREE_NODE*			pChilds[8];
		_bool					bCulled = { false };
	};

	_bool IsLeafNode(OCTREE_NODE* pNode){
		return nullptr == pNode->pChilds[0];
	}

};

NS_END