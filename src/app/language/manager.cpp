#include "manager.hpp"
#include "fs/fs.hpp"
#include <array>

#define NUM_LANGUAGES 2

static const char* Languages[NUM_LANGUAGES][LangItem::LANG_ITEM_COUNT] = {
	#include "lang/english.lang"
	#include "lang/polish.lang"
};

void LanguageManager::ListAll(OnListCB cb) {
	for (u32 i = 0; i < NUM_LANGUAGES; ++i) {
		cb(Languages[i][LangItem::LANG_ITEM_NAME], i);
	}
}

const char** LanguageManager::Get(u32 id) {
	if (id >= NUM_LANGUAGES) id = 0;
	return Languages[id];
}

