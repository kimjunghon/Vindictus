#pragma once

namespace Client
{
	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, END };
	enum class PROGRESS_TYPE { LOADING, HP, STAMINA };
	enum class GAMEPLAY_UI { DEFAULT, OPTION = 1<<1, CONTROLLER = 1<<2};
}
