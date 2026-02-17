namespace RE {
	class CombatController;
	class MagicCaster;

	class CombatAimController : public CombatObject
	{
	public:
		enum class PRIORITY : uint32_t
		{
			kUnk0,
			kUnk1,
			kUnk2,
			kUnk3,
			kUnk4,
			kUnk5
		};

		enum class Flags : uint32_t
		{
			kAiming = 1 << 0,
			kUpdating = 1 << 1,
			kUnk2 = 1 << 2,
			kDisable = 1 << 3,
			kUnk4 = 1 << 4,
			kUnk5 = 1 << 5,
			kUnk6 = 1 << 6
		};
		using FLAGS = stl::enumeration<Flags, uint32_t>;

		~CombatAimController();  // 00

		// override (CombatObject)
		std::uint32_t GetObjectType() override;                     // 02
		void          SaveGame(BGSSaveGameBuffer* a_buf) override;  // 03
		void          LoadGame(BGSLoadGameBuffer* a_buf) override;  // 04

		// add
		virtual bool                 CheckAim(const NiPoint3& from, const NiPoint3& to);  // 05
		virtual bool                 CheckAim(const NiPoint3& P);                         // 06
		virtual bool                 CheckAim(float cone);                                // 07
		virtual void                 Update();                                            // 08
		virtual CombatAimController* Clone() const;                                       // 09
		virtual void                 FinishLoadGame();                                    // 0A


		// members
		MagicCaster* mcaster;         // 10 -- or weap?
		NiPoint3          targetLoc;       // 18
		uint32_t          field_24;        // 24
		CombatController* combat_control;  // 28
		ActorHandle       target;          // 30
		PRIORITY          previousPriority;// 34
		PRIORITY          currentPriority; // 38
		FLAGS             flags;           // 3C
		AITimer           timer;           // 40
	private:
	};
	static_assert(sizeof(CombatAimController) == 0x48);
}