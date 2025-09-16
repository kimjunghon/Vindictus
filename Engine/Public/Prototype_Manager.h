#pragma once

#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Cell.h"
#include "Transform.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureUI.h"
#include "UI_Slot.h"
#include "UI_Panel.h"
#include "Model.h"
#include "Navigation.h"
#include "Collider.h"
#include "ColliderContainer.h"

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	typedef map<const _wstring, CBase*> PROTOTYPES;

private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void	Clear(_uint iClearLevel);

private:
	_uint							m_iNumLevels = {};
	PROTOTYPES*						m_pPrototypes = { nullptr };

private:
	CBase* Find_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);

public:
	static CPrototype_Manager*	Create(_uint iNumLevel);
	virtual void				Free() override;
};

NS_END