#pragma once

namespace RE
{
#pragma warning(push)
#pragma warning(disable: 4324)  // structure was padded due to alignment specifier

	class CombatBehaviorTreeNode;

	class CombatBehaviorContext
	{
	public:

		void Enter() {}
		void Exit() {}
	};

#pragma warning(pop)
}