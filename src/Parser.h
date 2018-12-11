#ifndef __PARSER_H__
#define __PARSER_H__

#include "Monster.h"

typedef enum JSONObjectType {
	eJSONObjectTypeNone,
	eJSONObjectTypeDictionaryEntry,
	eJSONObjectTypeDictionary,
	eJSONObjectTypeString,
	
} JSONObjectType;

typedef struct JSONObject {
	JSONObjectType sType;
} JSONObject;

typedef struct JSONObjectDictionaryEntry {
	JSONObjectType sType;
	char* key;
	JSONObject* value;
} JSONObjectDictionaryEntry;

typedef struct JSONObjectDictionary {
	JSONObjectType sType;
	JSONObjectDictionaryEntry** entries;
	uint16_t size;
	uint16_t max_size;
} JSONObjectDictionary;

typedef struct JSONObjectString {
	JSONObjectType sType;
	char* string;
} JSONObjectString;

JSONObjectDictionary* parse( const char* path );
void jsonify( char* path, JSONObjectDictionary* dict );

#endif
