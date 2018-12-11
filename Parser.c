#include "Monster.h"
#include "Parser.h"

#include <stdio.h>
#include <string.h>
#include <ncurses.h>

static char* many_tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

static void readFile( char* buffer, FILE* file ){
	char* cur_char = buffer;
	while( (*cur_char++ = fgetc( file )) != EOF );
	*--cur_char = '\0';
}

static int countChars( const char* buffer, const char* chars ){
	int num = 0;
	const char* i1 = buffer;
	const char* i2;
	while( *i1 ){
		i2 = chars;
		while( *i2 ){
			if( *i1 == *i2 ){
				num++;
				break;
			}
			i2++;
		}
		i1++;
	}
	return num;
}

static char* strcopy( const char* string ){
	char* newString = malloc( strlen( string ) + 1 );
	strcpy( newString, string );
	return newString;
}

static uint16_t tokenize( FILE* file, char*** return_value ){
	fseek( file, 0L, SEEK_END );
	char buffer[ ftell( file ) + 1 ];
	fseek( file, 0L, SEEK_SET );
	readFile( buffer, file );

	uint16_t tokens = countChars( buffer, "{}:," ) * 2 - 2;

	*return_value = (char**)malloc( tokens * sizeof( char* ));

	char buffertemp[4096] = "\0";

	char* temp = buffertemp;
	int index = 0;
	int inWord = 0;
	for( char* curr_char = buffer; *curr_char; curr_char++){
		switch( *curr_char ){
			case '{':
			case '}':
			case ',':
			case ':':
				if( inWord ){
					*temp = *curr_char;
					temp++;
				}else{
					(*return_value)[index] = malloc(2);
					*((*return_value)[index++]) = *curr_char;
				}
				break;
			case '"':
				if( inWord ){
					*temp = '\0';
					(*return_value)[index++] = strcopy( buffertemp );
					temp = buffertemp;
					inWord = 0;
				}else{
					inWord = 1;
				}
				break;
			case '\t':
			case '\r':
			case '\n':
				if( inWord ){
					*temp = *curr_char;
					temp++;
				}
				break;
			case ' ':
				if( inWord ){
					*temp = ' ';
					temp++;
				}
				break;
			default:
				if( inWord ){
					*temp = *curr_char;
					temp++;
				}
				else{
					fprintf( stderr, "Possible error during tokenization of the JSON. Are all names/strings encapsulated in \"? %s %u\n", curr_char, (uint32_t)*curr_char );
				}
		}
	}

	return index;
}

//Parses one Object, returns the point after the object
static uint16_t parseDict( char** content, uint16_t curr_token, JSONObjectDictionary* object ){
	if( strcmp( content[curr_token++], "{" )){	//token != {
		return curr_token - 1;					//return call value
	}
	//loop over all objects TODO once function finished, swap break with comma there or not
	while( strcmp( content[curr_token], "}" )){
		//Check if object has free space
		if( object->size == object->max_size ){
			object->max_size *= 2;
			object->entries = realloc( object->entries, sizeof( JSONObjectDictionaryEntry* ) * object->max_size );
		}
		object->entries[object->size++] = malloc( sizeof( JSONObjectDictionaryEntry ));
		object->entries[object->size - 1]->sType = eJSONObjectTypeDictionaryEntry;
		object->entries[object->size - 1]->key = strcopy( content[curr_token++] );

		if( strcmp( content[curr_token++], ":" )){
			fprintf( stderr, "WARNING: Error occured during parsing, found unexpected char %s\n", content[curr_token - 1] );
		}

		//Check if Object is a string or Dictionary
		if( strcmp( content[curr_token], "{" )){ //String
			JSONObjectString* string = malloc( sizeof( JSONObjectString ));
			string->sType = eJSONObjectTypeString;
			string->string = strcopy( content[curr_token++] );

			object->entries[object->size - 1]->value = (JSONObject*)string;
		}else {
			JSONObjectDictionary* dict = malloc( sizeof( JSONObjectDictionary ));
			dict->sType = eJSONObjectTypeDictionary;
			dict->size = 0;
			dict->max_size = 5;
			dict->entries = malloc( sizeof( JSONObjectDictionary* ) * dict->max_size );

			curr_token = parseDict( content, curr_token, dict );

			object->entries[object->size - 1]->value = (JSONObject*)dict;
		}
		
		if( !strcmp( content[curr_token], "," ))
			curr_token++;
		else if( strcmp( content[curr_token], "}" ))
			fprintf( stderr, "Found unexpected char %s. Expected '[\\{,]'.\n", content[curr_token] );
	}

	return ++curr_token;
}

JSONObjectDictionary* parse( const char* path ){
	FILE* json = fopen( path, "r" );

	char** tokens = NULL;

	int len = tokenize( json, &tokens );


	JSONObjectDictionary* dict = malloc( sizeof( JSONObjectDictionary ));
	dict->sType = eJSONObjectTypeDictionary;
	dict->size = 0;
	dict->max_size = 1;
	dict->entries = malloc( sizeof( JSONObjectDictionary* ) * dict->max_size );
	
	uint16_t last = parseDict( tokens, 0, dict );

	if( last != len ){
		fprintf( stderr, "WARNING: Syntax error in JSON\n" );
	}

	for( uint16_t i = 0; i < last; i++ )
		free( tokens[i] );

	fclose( json );

	return dict;
}

static void jsonifyObject( FILE* file, JSONObject* object, int ident ){
	switch( object->sType ){
		case eJSONObjectTypeNone:
			fprintf( stderr, "Found malformed JSONObject.\n" );
			break;
		case eJSONObjectTypeDictionary:
			{
				fprintf( file, "\n%.*s{", ident, many_tabs );
				for( int i = 0; i < ((JSONObjectDictionary*)object)->size; i++ ){
					JSONObjectDictionaryEntry* e = ((JSONObjectDictionary*)object)->entries[i];
					fprintf( file, "\n%.*s\"%s\": ", ident + 1, many_tabs, e->key );
					jsonifyObject( file, e->value, ident + 1 );
					if( i != ((JSONObjectDictionary*)object)->size - 1 )
						fprintf( file, "," );
				}
				fprintf( file, "\n%.*s}", ident, many_tabs );
			}
			break;
		case eJSONObjectTypeDictionaryEntry:
			fprintf( stderr, "Should never happen\n" );
			break;
		case eJSONObjectTypeString:
			fprintf( file, "\"%s\"", ((JSONObjectString*)object)->string );
			break;
	}
}

void jsonify( char* path, JSONObjectDictionary* dict ){
	FILE* json = fopen( path, "w" );

	jsonifyObject( json, (JSONObject*)dict, 0 );
	fflush( json );

	fclose( json );
}

