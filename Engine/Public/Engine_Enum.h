#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class STATE { RIGHT, UP, LOOK, POSITION };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class LAYERTYPE { STATIC, NONSTATIC, END};
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };
	enum class OBJECTGROUP { STATIC, DYNAMIC};
	enum class WINMODE { FULL, WIN, END };
	
	enum class MOUSEKEYSTATE { LB, RB, MB, END	};
	enum class MOUSEMOVESTATE {	X, Y, Z, END	};

	enum class SHADER_VTXPOSTEX { DEFAULT, PROGRESSBAR};
}

#endif // Engine_Enum_h__
