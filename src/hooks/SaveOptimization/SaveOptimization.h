#include <boost/unordered/unordered_flat_map.hpp>

class SaveOptimization : public HookTemplate<SaveOptimization>
{
public:
	static void Install();
	static void ResetCaches();

protected:
	static void SaveVM(void* thiz, RE::SaveStorageWrapper* save, RE::SkyrimScript::SaveFileHandleReaderWriter* writer, bool bForceResetState);
	static void SaveGame(RE::BGSSaveLoadGame* thiz, RE::Win32FileType* fileStream);
	static RE::BSStorageDefs::ErrorCode EnsureCapacity(RE::SaveStorageWrapper* thiz, unsigned __int64 size);

	static void UnloadStringTable(RE::BSScript::ReadableStringTable* thiz);
	static void ResetState(RE::BSScript::Internal::VirtualMachine* thiz);
	static bool StringTableSaveGame(RE::BSScript::WritableStringTable* thiz, RE::SaveStorageWrapper* save);
	static bool WriteString(RE::BSScript::WritableStringTable* thiz, RE::SaveStorageWrapper* save, RE::detail::BSFixedString<char>* scriptName);

	static unsigned int InsertFormID(RE::BGSSaveLoadFormIDMap* a1, RE::FormID formID_1);

	static inline REL::Relocation<decltype(SaveVM)>         _SaveVM;
	static inline REL::Relocation<decltype(SaveGame)>       _SaveGame;
	static inline REL::Relocation<decltype(EnsureCapacity)> _EnsureCapacity;

	static inline REL::Relocation<decltype(UnloadStringTable)> _UnloadStringTable;
	static inline REL::Relocation<decltype(ResetState)>        _ResetState;

	static inline REL::Relocation<decltype(InsertFormID)> _InsertFormID;

	static void Save(RE::BGSSaveLoadManager* thiz, unsigned int type, unsigned int a3, char* a4);
	static inline REL::Relocation<decltype(Save)> _Save;
};
