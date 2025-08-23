#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();
		return dwRefCnt;
	}

	template<typename T>
	struct PairHash
	{
		size_t operator()(const pair<T, T>& Pair) const noexcept
		{
			T Min = Pair.first > Pair.second ? Pair.second : Pair.first;
			T Max = Pair.first < Pair.second ? Pair.second : Pair.first;

			return hash<T>{}(Min) ^ (hash<T>{}(Max) << 1);
		}
	};

	template<typename T>
	struct PairEqual
	{
		_bool operator()(const pair<T, T>& PairA, const pair<T, T>& PairB) const noexcept
		{
			return (PairA.first == PairB.first && PairA.second == PairB.second) ||
				(PairA.first == PairB.second && PairA.second == PairB.first);
		}
	};

}

#endif // Engine_Function_h__
