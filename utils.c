#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ScopeNode* slptr = NULL; // left ptr of Scope list 
static ScopeNode* srptr = NULL; // right ptr of Scope list

/* helper functions */
Boolean isCommand(const char* command_line, int read) {
    Boolean ret_val = FALSE;
    if( (97 <= command_line[read] && command_line[read] <= 122) || command_line[read] == ' ' || (48 <= command_line[read] && command_line[read] <= 57) ) {
        ret_val = TRUE;
    }
    return ret_val;
}
Boolean inCommand(const char* command, const char* pattern) { 
    Boolean ret_val = FALSE;
    int i = 0;
    while(command[i] != '\0' && ret_val == FALSE) {
        int j = 0;
        ret_val = TRUE;
        while( pattern[j] != '\0' ) {
            ret_val = ret_val && (command[i+j] == pattern[j]);
            j++;
        }
        i++;
    }
    return ret_val;
}

/* core wrappers */
status_code BeginScope() {
    return NewScope();
}
status_code EndScope() {
    return DeleteScope();
}
status_code Assign(const char* command) {
    char varName[4]; data_t value = 0;
     
    /* decode varName and value from command */
    int read = 0;
    while( command[read] != ' ' ) {
        read = read + 1;
    }

    read = read + 1;
    int write = 0;
    while( (write < 3) && (command[read] != ' ') ) {
        varName[write] = command[read];
        write++; 
        read++;
    }
    varName[write] = '\0';

    read = read + 1;
    while( command[read] != '\0' ) {
        if( 48 <= command[read] && command[read] <= 57 ) {
            value = 10*value + (int)(command[read] - 48);
        }
        read = read + 1;
    }

    status_code sc;
    sc = InsertToHash(varName, value, (*srptr->symTable));
    return sc;
}
data_t Print(const char* command) {
    char varName[4];

    /* decoding varName from command */
    int read = 0;
    while( command[read] != ' ' ) {
        read++;
    }
    read = read + 1;

    int write = 0;
    while( (write < 3) && (97 <= command[read] && command[read] <= 122) ) {
        varName[write] = command[read];
        write++;
        read++;
    }
    varName[write] = '\0';

    /* Search for variable in the Scope list and catch its value in ret_val */
    data_t ret_val = DEFAULTVALUE;
    status_code sc = FAILURE; 
    ScopeNode* curr_scope = srptr;
    while( !sc && curr_scope != NULL ) {
        sc = AccessFromHash(varName, &ret_val, (*curr_scope->symTable));
        curr_scope = curr_scope->prev;
    }

    // append to output file
    FILE* file_ptr = fopen(OPFILEPATH, "a");
    fprintf(file_ptr, "%d\n", ret_val);
    fclose(file_ptr);

    return ret_val;
}

/* CRUD operations on hash table */
int hash(key_type key) {
    int index = 7; int i = 0;
    while( key[i] != ENDOFKEY ) {
        index = index*31 + (int)key[i];
        i = i + 1;
    }
    /* bringing index to subscriptable range */
    Boolean MIN_MAX = FALSE;
    if( MIN_MAX == TRUE ) {
        // using min-max normalization
        double idx = (double)index;
        idx = (( HASHSIZE * (idx - 314.0) )/(329683.0 - 314.0 + 1));
        index = (int)idx;
        
    }
    else {
        // using modulo operator
        index = index%HASHSIZE;
    }
    return index;

}
// Inserts if doesn't exist, else Re-assigns
status_code InsertToHash(key_type key, data_t data, hash_t hashTable) {
    status_code sc = SUCCESS;
    int index = hash(key);
    
    /* check if already exists */
    Boolean found = FALSE;
    varNode* vptr = hashTable[index];
    while( !found && vptr != NULL ) {
        if( keyCompare(vptr->varName, key) == TRUE ) { /* keys equal */
            found = TRUE;
        }
        else {
            vptr = vptr->next;
        }
    }
    if( found ) { /* exists. Hence update(reassign) value */
        vptr->data = data;
    }
    else { /* doesn't exist. Create new node and insert into chain (linked list) */
        varNode* vnptr = (varNode*)malloc(sizeof(varNode));
        if( vnptr == NULL ) sc = FAILURE;
        else {
            strcpy(vnptr->varName, key);
            vnptr->data = data;
            
            // insertion at start
            varNode* temp = hashTable[index];
            (hashTable[index]) = vnptr;
            vnptr->next = temp;
        }
    } /* end of conditional !found */

    return sc;
}
status_code AccessFromHash(key_type key, data_t* data_ptr, hash_t hashTable) {
    status_code sc = SUCCESS;
    int index = hash(key);

    // searching
    Boolean found = FALSE;
    varNode* vptr = hashTable[index];
    while( !found && vptr != NULL ) {
        if( keyCompare(vptr->varName, key) == TRUE ) { /* keys equal */
            found = TRUE;
        }
        else {
            vptr = vptr->next;
        }
    }

    if( !found ) sc = FAILURE;
    else { /* found */
        *data_ptr = vptr->data;
    }

    return sc;
}
void FreeHash(hash_t* hashTable_ptr) {

    varNode *prev, *curr;
    /* freeing each entry */
    for( int i = 0; i < HASHSIZE; i++ ) {
        prev = NULL;
        curr = (*hashTable_ptr)[i];
        while( curr != NULL ) {
            prev = curr;
            curr = curr->next;
            free(prev);
        }
        (*hashTable_ptr)[i] = NULL;
    }

    free(hashTable_ptr);
}
// returns TRUE if equal
Boolean keyCompare(key_type key1, key_type key2) {
    Boolean ret_val;
    if( strcmp(key1, key2) == 0 ) {
        ret_val = TRUE;
    }
    else {
        ret_val = FALSE;
    }
    return ret_val;
}

/* CRUD operations on scope list */
status_code NewScope() {
    status_code sc = SUCCESS;

    /* allocate on ScopeList */
    ScopeNode* snptr = (ScopeNode*)malloc(sizeof(ScopeNode));
    if( snptr != NULL ) {
        /* allocate and initialise symTable */
        snptr->symTable = (hash_t*)malloc(sizeof(hash_t));
        for( int i = 0; i < HASHSIZE; i++ ) {
            (*snptr->symTable)[i] = NULL; // * (deference) operator has lower precedence than -> (element selection through pointer) operator
        }
    }
    else {
        sc = FAILURE;
    }
    snptr->next = NULL;
    snptr->prev = srptr;
    // updating scope ptrs
    if( srptr != NULL ) {
        srptr->next = snptr;
        srptr = snptr;
    }
    if( slptr == NULL ) {
        slptr = snptr;
        srptr = snptr;
    }
    return sc;
}
status_code DeleteScope() {
    status_code sc = SUCCESS;
    if( srptr != NULL ) {
        ScopeNode* prev = srptr->prev;
        FreeHash(srptr->symTable);
        if( prev != NULL ) {
            prev->next= NULL;
        }
        else {
            slptr = NULL;
        }
        free(srptr);
        srptr = prev;

    }
    else { /* srptr == NULL */
        sc = FAILURE;
    }
    return sc;
}
status_code FreeScopeList() {
    status_code sc = SUCCESS;
    while( srptr != NULL && sc == SUCCESS) {
        sc = DeleteScope();
    }
    return sc;
}

/* display functions */
status_code fprint_all_symTables(const char* command) {
    status_code sc = SUCCESS;
    FILE* file_ptr = fopen("SymTable.txt", "a");
    if( file_ptr == NULL ) sc = FAILURE;
    else {
        ScopeNode* sptr = slptr;
        int scope_num = 1; varNode* vptr;
        fprintf(file_ptr, "after command: "); fputs(command, file_ptr); fprintf(file_ptr, "\n");
        while( sptr != NULL ) {
            fprintf(file_ptr, "\tscope: %d\n", scope_num);
            int idx = 0;
            while( idx < HASHSIZE ) {
                vptr = (*sptr->symTable)[idx];
                while( vptr != NULL ) {
                    fprintf(file_ptr,
                        "\t\tvarName: %s"
                        "\t\tdata: %d\n", 
                        vptr->varName,
                        vptr->data
                    );
                    vptr = vptr->next;
                }
                idx++;
            }
            sptr = sptr->next;
            scope_num = scope_num + 1;
        }
        fclose(file_ptr);
    }
    return sc;
}
void fPrintStatus(const char* opCode, status_code sc) {
    FILE* file_ptr;
    file_ptr = fopen(LOGFILEPATH, "a");

    fprintf(file_ptr, "\tOperation: "); fputs(opCode, file_ptr); fprintf(file_ptr, (sc==SUCCESS)?" status: SUCCESS\n":" status: FAILURE\n");
    fclose(file_ptr);
}
                           


    


