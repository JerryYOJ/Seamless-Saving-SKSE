namespace RE::BSScript {
	struct StringChunk
	{
		RE::BSFixedString strings[32];
		StringChunk* next;
	};

	struct WritableStringArray
	{
		RE::ScrapHeap* heap;
		RE::BSFixedString* _sentinel;
		RE::BSFixedString** tailPtr;
		RE::BSFixedString* headPtr;
		StringChunk* freeList;
		RE::BSFixedString* curPos;
		StringChunk* firstChunk;
		uint32_t count;
		uint32_t pad3C;
	};
}