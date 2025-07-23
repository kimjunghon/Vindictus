#pragma once

namespace Engine
{
	typedef struct tagModelInfo
	{
		unsigned int iModelType;
		unsigned int iNumMeshes;
		unsigned int iNumMaterials;

	}MODEL_INFO;

	typedef struct tagMeshInfo
	{
		unsigned int		iMaterialIndex;
		unsigned int		iNumVertices;
		unsigned int		iNumFaces;
	}MESH_INFO;
	

	typedef struct tagAnimMeshInfo : public tagMeshInfo
	{
		unsigned int iNumBones;
	}ANIMMESH_INFO;


}