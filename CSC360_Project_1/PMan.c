#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>   
#include <sys/wait.h>
#include "list.h"
#define MAX_MSG 50

char* first_word(const char* str); 
int compare_keywords(const char *str);
void bglist(ProcessList *list, char *tmsg[], int *num_tmsg);
//char* pstat();
//void bgkill();
//void bgstop();
//void bgstart();
void bg(char *str, ProcessList *list, char *tmsg[], int *num_tmsg, int *count);
ProcessList *plist = NULL;
int main(int argc, char const *argv[])
{   
    pid_t pid;
    int status;
    char inputline[100];
    char *tmsg[MAX_MSG];
    int num_tmsg = 0;
    ProcessList *list = create_list();
    int c = 0;

    while(1){
        printf("PMan:>");
        fgets(inputline, sizeof(inputline), stdin);//get user input
        
        //error handling of none input
        if (inputline[0] == '\n') {
        printf("Error: No input provided\n");
        continue;
        }

        char* command = first_word(inputline);//works
        int result = compare_keywords(command);//works

        if (result == 0){//error handling of invalid input
            int len = strlen(inputline);
            if (inputline[len - 1] == '\n') {
                inputline[len - 1] = '\0';
            }
            printf("%s: command not found\n", inputline);
        }else{
            pid = fork();
            //printf("inputline is: %s\n", inputline);
            if (pid < 0){
                fprintf(stderr, "Fork Failed\n");
            }
            else if (pid == 0){
                switch (result) {
                    case 1:
                        printf("bglistcount: %d\n", c);
                        printf("bglistpointer: %p\n", list);
                        bglist(list, tmsg, &num_tmsg);
                        break;
                    case 2:
                        printf("pstat\n");
                        break;
                    case 3:
                        printf("bgkill\n");
                        break;
                    case 4:
                        printf("bgstart\n");
                        break;
                    case 5:
                        printf("bgstop\n");
                        break;
                    case 6:
                        
                        inputline[strlen(inputline) - 1] = '\0';
                        bg(inputline, list, tmsg, &num_tmsg, &c);
                        //print_all(list);this not work
                        printf("after bg");
                        break;
                    default:
                        printf("None of the keywords\n");
                        break;
                }
                return 0;
            }
            else{
                // parent process
                usleep(30000);
                waitpid(-1, &status, WNOHANG);
                }
            }
    }

    return 0;
}

/*This function determines if the first word in a string 
is one of the following: bglist, pstat, bgkill, bgstart, bgstop,
 bg, or none of these. It avoids multiple spaces in the string
 parameter: a string
 return: the first word in string
*/
char* first_word(const char* str) {
    static char word[10];
    int i, j = 0;

    // Skip any leading whitespaces
    while (isspace(*str)) str++;

    // Copy the first word into the word buffer
    while (*str && !isspace(*str)) word[j++] = *str++;
    word[j] = '\0';

    // Return the first word
    return word;
}

/*This function compares the input string str with 
each keyword. If a full match is found, the corresponding integer 
value is returned. If no match is found, the function returns 0
1:bglist
2:pstat
3:bgkill
4:bgstart
5:bgstop
6:bg
*/
int compare_keywords(const char *str) {


    if (strcmp(str, "bglist") == 0) return 1;
    if (strcmp(str, "pstat") == 0) return 2;
    if (strcmp(str, "bgkill") == 0) return 3;
    if (strcmp(str, "bgstart") == 0) return 4;
    if (strcmp(str, "bgstop") == 0) return 5;
    if (strcmp(str, "bg") == 0) return 6;
    return 0;
}

/*This function manage commands in "pg arg" form*/
void bg(char *str, ProcessList *list, char *tmsg[], int *num_tmsg, int *c) {
    int id = getpid();
    // Find the first non-whitespace character in the string
    int i = 0;
    printf("bgpointer: %p\n", list);
    while (isspace(str[i])) i++;

    // Check if the first word is "bg"
    if (str[i] == 'b' && str[i + 1] == 'g' && !isalpha(str[i + 2])) {
        // Remove "bg" from the string
        int j = i + 2;
        while (str[j]) {
            str[j - 2] = str[j];
            j++;
        }
        str[j - 2] = '\0';
    } 
    printf("process %d created for %s program...\n", id, str);
    add_process(list, id, str);
    c++;
    plist = list;
    printf("test4:%s\n", plist->head->filename);
    print_all(list); //this work
    get_count(list);
    char* args[100];
    int count = 0;
    char* token = strtok(str, " ");
    while (token != NULL) {
        args[count] = token;
        count++;
        token = strtok(NULL, " ");
    }
    args[count] = NULL;
    execvp(args[0], args);
    // execvp only returns if there is an error
    perror(str);
    //add a terminated message to this pid
    add_terminated(list, id, "failed to execute");

    remove_process(list, id);
    //printf("msg from removed: %s\n", msg); //work
    
    exit(1);
}

void bglist(ProcessList *list, char *tmsg[], int *num_tmsg){
    //printf("test5: %s", list->head->filename);
    get_count(list);
    print_all(list);
    print_msg(list);
    
    //Print the terminated messages if there are any   
}
