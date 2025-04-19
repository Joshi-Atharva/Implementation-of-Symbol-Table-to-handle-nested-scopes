#include <stdio.h>
#include <string.h>
#include "utils.h"

int main() {
    FILE* file_ptr; FILE* symTableFile_ptr; FILE* logFile_ptr;
    /* clears the output, symTable and log file for fresh writing */
    file_ptr = fopen(OPFILEPATH, "w");
    fclose(file_ptr);
    symTableFile_ptr = fopen(SYMTABLEFILEPATH, "w");
    fclose(symTableFile_ptr);
    logFile_ptr = fopen(LOGFILEPATH, "w");
    fclose(logFile_ptr);


    FILE* input_file;
    input_file = fopen(IPFILEPATH, "r");
    if( input_file == NULL ) {
        printf("cannot open input file\n");
    }
    else {
        char command[100], command_line[100];
        status_code sc;
        while( !feof(input_file) ) { 
            fflush(input_file);
            fgets(command_line, 100, input_file);
            printf("read string: "); puts(command_line);
            int read = 0; int write = 0;
            Boolean begin_flag = FALSE;
            while(isCommand(command_line, read)) {
                if( !begin_flag && (97 <= command_line[read] && command_line[read] <= 122) ) {
                    begin_flag = TRUE;
                }
                if( begin_flag == TRUE ) {
                    command[write] = command_line[read];
                    write++;
                }
                read++;
            }
            command[write] = '\0';
            printf("command string: "); puts(command); printf("\n");
            if( inCommand(command, "begin") == TRUE ) {
                sc = BeginScope(); fPrintStatus(command, sc); 
            }
            else if( inCommand(command, "end") == TRUE ) { 
                sc = EndScope(); fPrintStatus(command, sc);
            }
            else if( inCommand(command, "assign") == TRUE ) {
                sc = Assign(command); fPrintStatus(command, sc);
            }
            else if( inCommand(command, "print") == TRUE ) {
                Print(command); fPrintStatus(command, SUCCESS);
            }
            fprint_all_symTables(command);
        }
    }

    fclose(input_file);
    return 0;
}