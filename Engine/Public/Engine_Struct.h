#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE		hInst;
		HWND			hWnd;
		WINMODE			eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;
		unsigned int	iNumLevels;
	}ENGINE_DESC;	

	typedef struct tagInputMoveDesc 
	{
		XMVECTOR	vDir;
		bool		bMove;
		bool		bSprint;
	}INPUT_MOVE_DESC;

	typedef struct tagInputActionDesc 
	{
		bool		bAction;
		byte		byAction;
	}INPUT_ACTION_DESC;

	typedef struct tagInputCameraDesc
	{
		bool		bRotate;
		XMFLOAT2	vCameraRotate;
		float		fDistance;
	}INPUT_CAMERA_DESC;

	typedef struct tagInputUIDesc 
	{
		bool		bOption;
		bool		bMouse;
		bool		bInventory;
	}INPUT_UI_DESC;

	typedef struct tagUIStateDesc
	{
		unsigned int* iUIState;
	}UI_STATE_DESC;

	typedef struct tagLightDesc
	{
		enum TYPE { DIRECTIONAL, POINT, END};

		TYPE		eType;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		XMFLOAT4	vPosition;
		XMFLOAT4	vDirection;
		float		fRange;
	}LIGHT_DESC;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;
		float		fTrackPosition;
	}KEYFRAME;

	typedef struct tagVertexPosition
	{
		XMFLOAT3		vPosition;

		static const unsigned int iNumElements = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXPOS;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXPOSTEX;

	typedef struct tagVertexNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXNORTEX;

	typedef struct tagVertexPositionColor
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vColor;

		static const unsigned int iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXNORCOL;

	typedef struct tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = { 5 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXMESH;

	typedef struct tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXANIMMESH;

	typedef struct tagAnimData
	{
		string		strAnimKey;
		bool		IsLoop;
		XMFLOAT2	vRange;
		float		fAnimSpeed;
	}ANIM_DATA;

	typedef struct tagRootMotionOption
	{
		bool		PositionX;
		bool		PositionY;
		bool		PositionZ;

		bool		Rotation;
	}ROOTMOTION_OPTION;

	typedef struct tagModelBounding
	{
		XMFLOAT3 vMinPosition;
		XMFLOAT3 vMaxPosition;
	}MODEL_BOUNDING;
}


#endif // Engine_Struct_h__
