#pragma once

namespace Client
{
	namespace UI
	{
		enum class STATE_FLAG {
			LOADING =	1 << 0,
			LOGO =		1 << 1,
			GAMEPLAY =  1 << 2
		};

		enum class LOADING_FLAG {
			LOGO =			1 << 4,
			TOWN =			1 << 5,
			FIELD =			1 << 6,
			QUEEN =			1 << 7,
			GLASGAVELEN =	1 << 8
		};

		enum class GAMEPLAY_FLAG {
			DEFAULT =		1 << 4,
			OPTION =		1 << 5,
			CONTROLLER =	1 << 6,
		};
	}

}