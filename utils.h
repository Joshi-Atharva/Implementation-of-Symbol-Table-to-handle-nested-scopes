#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HASHSIZE 1000
#define ENDOFKEY '\0'
#define DEFAULTVALUE 0
#define OPFILEPATH "OutputFile.txt"
#define IPFILEPATH "InputFile.txt"
#define SYMTABLEFILEPATH "SymTable.txt"
#define LOGFILEPATH "Log.txt"
typedef int data_t;
typedef char* key_type;
typedef enum{FAILURE, SUCCESS} status_code;
typedef enum{FALSE, TRUE} Boolean;

typedef struct attribute_tag {
    data_t data;
}attribute_t;

typedef struct varNode_tag {
    char varName[4];
    data_t data;
    struct varNode_tag* next;
}varNode;

typedef varNode* hash_t[HASHSIZE];

typedef struct ScopeNode_tag {  
    hash_t* symTable;
    struct ScopeNode_tag *prev;
    struct ScopeNode_tag *next;
}ScopeNode;

Boolean isCommand(const char* command_line, int read);
Boolean inCommand(const char* command, const char* pattern);
/* core wrappers */
status_code BeginScope();
status_code EndScope();
status_code Assign(const char* command);
data_t Print(const char* command);

/* CRUD operations on hash table */
int hash(key_type key);
// Inserts if doesn't exist, else Re-assigns
status_code InsertToHash(key_type key, data_t data, hash_t hashTable);
status_code AccessFromHash(key_type key, data_t* data_ptr, hash_t hashTable);
void FreeHash(hash_t* hashTable_ptr);
// returns TRUE if equal
Boolean keyCompare(key_type key1, key_type key2);


/* CRUD operations on scope list */
status_code NewScope();
status_code DeleteScope();
status_code FreeScopeList();

/* display functions */
status_code fprint_all_symTables(const char* command);
void fPrintStatus(const char* opCode, status_code sc);

#endif