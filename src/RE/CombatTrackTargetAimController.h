namespace RE {
	class CombatAimController;
	class MagicCaster;

	class CombatTrackTargetAimController : public CombatAimController
	{
	public:
		~CombatTrackTargetAimController();

		// override (CombatObject)
		std::uint32_t GetObjectType() override;                     // 02
		void          SaveGame(BGSSaveGameBuffer* a_buf) override;  // 03
		void          LoadGame(BGSLoadGameBuffer* a_buf) override;  // 04

		// override (CombatAimController)
		void                 Update() override;       // 08
		CombatAimController* Clone() const override;  // 09

		// members
		float   trackDistanceMax;  // 48
		bool    field_4C;          // 4C
		uint8_t pad4D[3];          // 4D
	private:
	};
	static_assert(sizeof(CombatTrackTargetAimController) == 0x50);
}