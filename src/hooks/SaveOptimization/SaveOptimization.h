#include <boost/unordered/unordered_flat_map.hpp>

class SaveOptimization : public HookTemplate<SaveOptimization> {
public:
	static void Install();
	static void ResetCaches();
protected:
	//static void AddString(RE::BSScript::WritableStringTable* thiz, RE::BSFixedString* str);
	//static bool LoadTypeTable(RE::BSTHashMap<RE::BSFixedString, RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo>>* thiz, RE::LoadStorageWrapper* loader, RE::BSScript::IVMSaveLoadInterface* intfc, RE::BSScript::Internal::VirtualMachine* vm);
	static void SaveVM(RE::SkyrimVM* thiz, RE::SaveStorageWrapper* save);
	static void SaveGame(RE::BGSSaveLoadGame* thiz, RE::Win32FileType* fileStream);
	static RE::BSStorageDefs::ErrorCode EnsureCapacity(RE::SaveStorageWrapper* thiz, unsigned __int64 size);

	static void UnloadStringTable(RE::BSScript::ReadableStringTable* thiz);
	static void ResetState(RE::BSScript::Internal::VirtualMachine* thiz);
	static bool StringTableSaveGame(RE::BSScript::WritableStringTable* thiz, RE::SaveStorageWrapper* save);
	static bool WriteString(RE::BSScript::WritableStringTable* thiz,RE::SaveStorageWrapper* save,RE::detail::BSFixedString<char>* scriptName);

	static inline REL::Relocation<decltype(SaveVM)> _SaveVM;
	static inline REL::Relocation<decltype(SaveGame)> _SaveGame;
	static inline REL::Relocation<decltype(EnsureCapacity)> _EnsureCapacity;

	static inline REL::Relocation<decltype(UnloadStringTable)> _UnloadStringTable;
	static inline REL::Relocation<decltype(ResetState)> _ResetState;
	//static inline REL::Relocation<decltype(StringTableSaveGame)> _StringTableSaveGame;
	//static inline REL::Relocation<decltype(WriteString)> _WriteString;

};