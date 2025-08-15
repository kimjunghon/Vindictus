#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLayer final : public CBase
{
private:
    CLayer();
    virtual ~CLayer() = default;

public:
    class CComponent*   Get_Component(const _wstring& strComponentTag, _uint iIndex);
    HRESULT             Add_GameObject(class CGameObject* pGameObject) {
        m_GameObjects.push_back(pGameObject);
        return S_OK; }

public:
    void Post_Update(_float fTimeDelta);
    void Priority_Update(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Late_Update(_float fTimeDelta);

private:
    list<class CGameObject*>    m_GameObjects;
    unordered_set<_uint>        m_NoneActiveObjectIndices;

public:
    static CLayer*  Create();
    virtual void    Free() override;
};

NS_END