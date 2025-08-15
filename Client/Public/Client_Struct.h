#pragma once

namespace Client
{
	typedef struct tagPlayerStatus {
		float fFullHealth;
		float fHealth;
		float fStamina;
	}PLAYER_STATUS;

	typedef struct tagSlotRenderDesc {
		XMFLOAT4X4 WorldMatrix;
		XMFLOAT4X4 ViewMatrix;
		XMFLOAT4X4 ProjMatrix;
	}SLOT_RENDER_DESC;

	typedef struct tagMonsterSpawnData {
		_uint		iRoomIndex;
		string		strMonsterName;
		int			iCellIndex;
		XMFLOAT3	vPosition;
	}MONSTER_SPAWN_DATA;

}