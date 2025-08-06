#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class D3DRS { DEFAULT, CCW, NONCULL, WIRE, END};
	enum class D3DBS { DEFAULT, ALPHABLEND, END };
	enum class D3DDSS { DEFAULT, ZDISABLE, END};
	enum class D3DTS { VIEW, PROJ, END };
	enum class STATE { RIGHT, UP, LOOK, POSITION };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class LAYERTYPE { STATIC, NONSTATIC, END};
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };
	enum class OBJECTGROUP { STATIC, DYNAMIC};
	enum class WINMODE { FULL, WIN, END };
	
	enum class ACTION { ATTACK = 1, SMASH = 1 << 1, };

	enum class MOUSEKEYSTATE { LB, RB, MB, END	};
	enum class MOUSEMOVESTATE {	X, Y, WHEEL, END	};

	enum class CONTROLLER_CHANNEL { MAIN, UI, END};
	enum class UI_DEPTH { FIRST = 2, SECOND, THIRD, FORTH, FIFTH};

	enum class SHADER_VTXPOSTEX { DEFAULT, PROGRESSBAR, LOADINGPOINT};

	enum class MODELTYPE { ANIM, NONANIM, INFILE };

	enum class BT_STATE { RUN, SUCCESS, FAILED };

	enum class CELL_POINT { A, B, C, END };
	enum class LINE { AB, BC, CA, END};
}

#endif // Engine_Enum_h__
