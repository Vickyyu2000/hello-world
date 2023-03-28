#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>

ProcessList *create_list()
{
    ProcessList *plist = (ProcessList *)malloc(sizeof(ProcessList));
    plist->head = NULL;
    plist->count = 0;
    plist->num_tmsg = 0;
    return plist;
}

/*add new process at the start of the list*/
void add_process(ProcessList *plist, int pid, char *filename)
{
    Process *new_process = (Process *)malloc(sizeof(Process));
    new_process->pid = pid;
    strcpy(new_process->filename, filename);
    printf("test: %s\n", new_process->filename);
    new_process->next = plist->head;
    plist->head = new_process;
    printf("test2:%s\n", plist->head->filename);
    plist->count++;
}

/*remove a process and return its terminated msg*/
void *remove_process(ProcessList *plist, int pid)
{
    Process *curr = plist->head, *prev = NULL;
    while (curr != NULL)
    {
        if (curr->pid == pid)
        {
            if (prev == NULL)
                plist->head = curr->next;
            else
                prev->next = curr->next;

            char *msg = curr->terminated_msg;
            plist->tmsg[(plist->num_tmsg)++] = msg;
            free(curr);
            plist->count--;
            //return terminated_msg;

        }
        prev = curr;
        curr = curr->next;
    }
    return NULL;
}

/*add a terminated msg to the corresponding process*/
void add_terminated(ProcessList *plist, int pid, char *terminated_msg)
{
    Process *curr = plist->head;
    while (curr != NULL)
    {
        if (curr->pid == pid)
        {
            strcpy(curr->terminated_msg, terminated_msg);
            break;
        }
        curr = curr->next;
    }
}

char *get_filename(ProcessList *plist, int pid)
{
    Process *curr = plist->head;
    while (curr != NULL)
    {
        if (curr->pid == pid)
            return curr->filename;
        curr = curr->next;
    }
    return NULL;
}

char *get_msg(ProcessList *plist, int pid)
{
    Process *curr = plist->head;
    while (curr != NULL)
    {
        if (curr->pid == pid)
            return curr->terminated_msg;
        curr = curr->next;
    }
    return NULL;
}

/*return the total number of process in the list*/
int get_count(ProcessList *plist)
{
    printf("Total backgrond jobs: %d\n", plist->count);
}

void print_all(ProcessList *plist)
{
    Process *curr = plist->head;
    while (curr != NULL)
    {
        printf("PID: %d    Filename: %s\n", curr->pid, curr->filename);
        curr = curr->next;
    }
}

void print_msg(ProcessList *plist){
    int count = plist->num_tmsg;
    for (int i = 0; i < count; i++) {
        printf("%s\n", plist->tmsg[i]);
        //free(plist->tmsg[i]);
        plist->tmsg[i] = NULL;
    }
    plist->num_tmsg = 0;
}
