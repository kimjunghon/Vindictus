#pragma once

#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "Shader.h"
#include "Texture.h"

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