#include "SaveOptimization.h"

#include "../../RE/WritableStringArray.h"
#include "../../RE/WriteBuffer.h"

#include <MinHook.h>
#include <future>

std::future<RE::WriteBuffer> vmSave;

std::vector<RE::BSFixedString> StringTableCache;
boost::unordered_flat_map<const char*, uint32_t> StringTableCacheLookup;

static RE::SaveStorageWrapper* Ctor(void* svWrapperSpace, RE::Win32FileType* fileStream, uint64_t size) {
    static REL::Relocation<void(*)(void* thiz, RE::Win32FileType* file)>Ctor{ RELOCATION_ID(35172, 36062) };
    Ctor(svWrapperSpace, fileStream);
    RE::SaveStorageWrapper* svWrapper = (RE::SaveStorageWrapper*)svWrapperSpace;
    RE::MemoryManager::GetSingleton()->GetThreadScrapHeap()->Deallocate(((RE::WriteBuffer*)svWrapper->unk10)->startPtr);

    auto&& writebuf = ((RE::WriteBuffer*)svWrapper->unk10);

	void* rawMem = malloc(size);
    if(!rawMem) {
        logger::critical("Failed to allocate memory for save buffer. Requested size: {} bytes", size);
        *((volatile int*)0xDEAD0002) = 0; //Force a crash to avoid corrupting save data, should be visible in crash logs as well
        return svWrapper;
	}

    writebuf->startPtr = rawMem;
    writebuf->size = size;
    writebuf->curPtr = writebuf->startPtr;
    svWrapper->unk18 = 1; //bWriteToBuffer

    return svWrapper;
}

//MUST SAVE STARTPTR AND FREE THEN ELSEWHERE
static void Dtor(RE::SaveStorageWrapper* svWrapper) {
    auto&& writebuf = ((RE::WriteBuffer*)svWrapper->unk10);
    writebuf->startPtr = nullptr;
    writebuf->size = 0;
    writebuf->curPtr = nullptr;
    
    static REL::Relocation<void(*)(RE::SaveStorageWrapper*)>Dtor{ RELOCATION_ID(35173, 36063) };
    Dtor(svWrapper);
}

DWORD vmSaveThreadID = 0;

void SaveOptimization::Install()
{
    {  //Multithrad VM Save
        REL::Relocation<LPVOID>savevm{ RELOCATION_ID(34732, 35638), REL::VariantOffset(0x11A, 0x11A, 0) };
        _SaveVM = SKSE::GetTrampoline().write_call<5>(savevm.address(), SaveVM);

        REL::Relocation<LPVOID>savegame{ RELOCATION_ID(34676, 35599) };
        MH_CreateHook(savegame.get(), SaveGame, (LPVOID*)&_SaveGame);

        REL::Relocation<LPVOID>ensurecap{ RELOCATION_ID(20154, 20154) }; //buffer growth
        MH_CreateHook(ensurecap.get(), EnsureCapacity, (LPVOID*)&_EnsureCapacity);
    }
    
    {   //Stringtable caching
	    REL::Relocation<LPVOID>dtorstrtable{ RELOCATION_ID(98106, 104829), REL::VariantOffset(0xAF2, 0xAE8, 0)};
		_UnloadStringTable = SKSE::GetTrampoline().write_call<5>(dtorstrtable.address(), UnloadStringTable);
    
        //Hook ResetState
        REL::Relocation<LPVOID>resetstate{ RELOCATION_ID(98158, 104882) };
		MH_CreateHook(resetstate.get(), ResetState, (LPVOID*)&_ResetState);
        
        //Hook WritableStringTable::SaveGame
		REL::Relocation<LPVOID>strsavegame{ RELOCATION_ID(97947, 104679)};
		MH_CreateHook(strsavegame.get(), StringTableSaveGame, nullptr);
        
        //Hook WriteStrings
		REL::Relocation<LPVOID>writestr1{ RELOCATION_ID(97948, 104680) };
		REL::Relocation<LPVOID>writestr2{ RELOCATION_ID(97949, 104681) };
        REL::Relocation<LPVOID>writestr3{ RELOCATION_ID(97950, 104682) };
		MH_CreateHook(writestr1.get(), WriteString, nullptr);
        MH_CreateHook(writestr2.get(), WriteString, nullptr);
        MH_CreateHook(writestr3.get(), WriteString, nullptr);
        //Put off lifting typetable first, focus on stringtable caching
    }

    {// Fix a race condition crash
		REL::Relocation<LPVOID>insertformid{ RELOCATION_ID(34634, 35554) };
        MH_CreateHook(insertformid.get(), InsertFormID, (LPVOID*)&_InsertFormID);
    }

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* msg) {
        if (msg->type == SKSE::MessagingInterface::kNewGame) {
            ResetCaches();
        }
	});
}

void SaveOptimization::ResetCaches()
{
    StringTableCache.clear();
	StringTableCacheLookup.clear();
	StringTableCache.push_back(""); //Empty string is always 0 in ida
	StringTableCacheLookup.emplace("", 0);
}

void SaveOptimization::SaveVM(RE::SkyrimVM* thiz, RE::SaveStorageWrapper* save) {
    auto&& writebuf = vmSave.get();

    save->unk18 = 0; //bWriteToBuffer
	save->Write(2, (std::byte*)writebuf.startPtr); //Version NUM
    
    //Saving Stringtable
	auto&& table = RE::BSScript::Internal::VirtualMachine::GetSingleton();
    uint32_t count = StringTableCache.size();
	save->Write(4, (std::byte*)&count);
	for (auto&& str : StringTableCache) {
        uint16_t len = str.length();
		save->Write(2, (std::byte*)&len);
        if (len > 0) save->Write(len, (std::byte*)str.data());
    }
    //Rest of Data
    save->Write((uint64_t)writebuf.curPtr - (uint64_t)writebuf.startPtr - 2, (std::byte*)writebuf.startPtr + 2);
    save->unk18 = 1;

    free(writebuf.startPtr);
    return;
}

void SaveOptimization::SaveGame(RE::BGSSaveLoadGame* thiz, RE::Win32FileType* fileStream) {
    auto promise = std::make_shared<std::promise<RE::WriteBuffer>>();
    vmSave = promise->get_future();

    std::thread([promise, fileStream] {
        vmSaveThreadID = GetCurrentThreadId();

        char svWrapperSpace[0x38]{};
        RE::SaveStorageWrapper* svWrapper = Ctor(&svWrapperSpace, fileStream, 64 * 1024 * 1024);
		auto&& writebuf = ((RE::WriteBuffer*)svWrapper->unk10);

        _SaveVM(RE::SkyrimVM::GetSingleton(), svWrapper);

        promise->set_value({writebuf->size, writebuf->startPtr, writebuf->curPtr});

        Dtor(svWrapper);

        vmSaveThreadID = 0;
    }).detach();

    return _SaveGame(thiz, fileStream);
}

RE::BSStorageDefs::ErrorCode SaveOptimization::EnsureCapacity(RE::SaveStorageWrapper* thiz, unsigned __int64 size) {
    if(GetCurrentThreadId() != vmSaveThreadID) return _EnsureCapacity(thiz, size);
    
    auto&& writebuf = ((RE::WriteBuffer*)thiz->unk10);
    size_t used = (uint64_t)writebuf->curPtr - (uint64_t)writebuf->startPtr;
    size_t available = writebuf->size - used;

    if (available < size) {
        size_t newSize = writebuf->size;
        while (newSize - used < size) {
            newSize *= 2;
        }

        void* newBuf = malloc(newSize);
        if (!newBuf) {
			logger::critical("Failed to allocate memory for save buffer expansion. Requested size: {} bytes", newSize);
			*((volatile int*)0xDEAD0001) = 0; //Force a crash to avoid corrupting save data, should be visible in crash logs as well
            return (RE::BSStorageDefs::ErrorCode)1;
        }

        memcpy(newBuf, writebuf->startPtr, used);
        free(writebuf->startPtr);

        writebuf->startPtr = newBuf;
        writebuf->curPtr = (char*)newBuf + used;
        writebuf->size = newSize;
    }
	return (RE::BSStorageDefs::ErrorCode)0;
}

void SaveOptimization::UnloadStringTable(RE::BSScript::ReadableStringTable* thiz)
{
    ResetCaches();
    StringTableCacheLookup.reserve(thiz->entries->size() * 1.5);
	StringTableCache.reserve(thiz->entries->size() * 1.5);

    for (auto&& it : *thiz->entries) {
        RE::BSFixedString str = std::move(it.convertedString);
        if (str.empty())
            //if(it.originalData != nullptr) str = it.originalData;
            //else continue;
            continue;
        
        StringTableCacheLookup.emplace(str.data(), StringTableCache.size());
        StringTableCache.push_back(std::move(str));
    }
    
    return _UnloadStringTable(thiz);
}

void SaveOptimization::ResetState(RE::BSScript::Internal::VirtualMachine* thiz) {
	thiz->unk94D0 = thiz->arrays.size(); //arrayCount

    if (!thiz->writeableTypeTable) {
        auto&& scrapheap = RE::MemoryManager::GetSingleton()->GetThreadScrapHeap();
        void* rawMem = scrapheap->Allocate(0x38, 8);
        *(RE::ScrapHeap**)rawMem = scrapheap;

        auto* table = (RE::BSTScrapHashMap<RE::BSFixedString, RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo>>*)((char*)rawMem + 0x8);
        std::construct_at(table);

        thiz->writeableTypeTable = (RE::BSTHashMap<RE::BSFixedString, RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo>>*)table;
    }
    auto&& typeTable = (RE::BSTScrapHashMap<RE::BSFixedString, RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo>>*)thiz->writeableTypeTable;
	typeTable->clear();
	typeTable->reserve(thiz->objectTypeMap.size());
    thiz->unk94CC = 0; //scriptCount
    for (auto&& obj : thiz->attachedScripts) {
        for (auto&& script : obj.second) {
            auto typeInfo = script->type;
            while (typeInfo != nullptr) {
                auto&& ins = typeTable->insert({ typeInfo->name, typeInfo });
				if (ins.second == false) break;
                typeInfo = typeInfo->parentTypeInfo;
            }
			thiz->unk94CC++; //scriptCount
        }
    }
    for (auto&& obj : thiz->objectsAwaitingCleanup) {
        auto typeInfo = obj->type;
        while (typeInfo != nullptr) {
			auto&& ins = typeTable->insert({ typeInfo->name, typeInfo });
            if (ins.second == false) break;
			typeInfo = typeInfo->parentTypeInfo;
        }
    }
    
    return;
}

bool SaveOptimization::StringTableSaveGame(RE::BSScript::WritableStringTable* thiz, RE::SaveStorageWrapper* save)
{
    return true;
}

bool SaveOptimization::WriteString(RE::BSScript::WritableStringTable* thiz, RE::SaveStorageWrapper* save, RE::detail::BSFixedString<char>* scriptName)
{
	if (!scriptName || !scriptName->data()) return false;
    
    auto&& it = StringTableCacheLookup.try_emplace(scriptName->data(), StringTableCache.size());
    if (it.second) {
        StringTableCache.push_back(*scriptName);
		//logger::info("Cache Miss: {} : {}", scriptName->data(), it.first->second);
    }
	//else logger::info("Cache Hit: {} : {}", scriptName->data(), it.first->second);

	if (thiz->indexSize.underlying() == 1) return save->Write(4, reinterpret_cast<const std::byte*>(&it.first->second)) == (RE::BSStorageDefs::ErrorCode)0;
    else {
		const uint16_t id = static_cast<uint16_t>(it.first->second);
        return save->Write(2, reinterpret_cast<const std::byte*>(&id)) == (RE::BSStorageDefs::ErrorCode)0;
    }
}

std::atomic_flag formIDLock = ATOMIC_FLAG_INIT;

unsigned int SaveOptimization::InsertFormID(RE::BGSSaveLoadFormIDMap* thiz, RE::FormID formID)
{
    while (formIDLock.test_and_set(std::memory_order_acquire)) {
        _mm_pause();
    }

    uint32_t result = _InsertFormID(thiz, formID);

    formIDLock.clear(std::memory_order_release);
    return result;
}