
------------------------------------------------FILE DESCRIPTION-------------------------------------------
list.h: 
    header file of list.c

list.c: 
    This code is a collection of functions that operate on a linked list data structure to 
    manage processes. The linked list is implemented using a structure named "ProcessList", 
    which contains a pointer to the head of the list and an integer that stores the number 
    of nodes in the list.

    The functions include:

        create_list(): creates an instance of the ProcessList structure and returns a pointer to it.

        add_process(): adds a new process node to the start of the linked list. The node contains 
        the process ID and the filename associated with the process.

        remove_process(): removes a process node from the linked list with a specified process ID.

        get_filename(): returns the filename associated with a specified process ID.

        get_count(): returns the number of nodes in the linked list.

        print_all(): prints all the nodes in the linked list, including the process ID and the filename
        associated with the process.

PMan.c:
    This program implements a simple process manager in C language. It has a command-line interface where the user
    can input different commands to manage background processes. The user can input commands such as "bglist",
    "pstat", "bgkill", "bgstart", "bgstop", and "bg".

Makefile:
    This is a makefile for building a C program named "PMan" that implements a simple process manager. 

------------------------------------------------FILE DESCRIPTION-------------------------------------------

---------------------------------------------------HOW TO USE----------------------------------------------
STEPS:

    1. Type 'make', a executable programs called PMan will be build

    2. Type ./PMan, the program will starting to run. It prompts 'PMan: >' and wait for further user input.
        
        Availiable Commands:
         
        'PMan: >bg': 
            PMan will start running the provided file (assuming at the same directory) after pg and display 
            the corresponding pid, else report an error. It display a termination message immediately if the 
            process terminates.
                
            example: 'PMan: >bg ./foo'

        'PMan: >bglist': 
            It displays a total number and a list of background processes.

        'PMan: >bgkill <pid>': 
            It kills a particular started process with a valid pid, else returns an error.

            example: 'PMan: >bgkill 1234'

        'PMan: >bgstop <pid>':
            It stops a particular running process, else returns an error.

            example: 'PMan: >bgstop 1234'
        
        'PMan: >bgstart <pid>':
            It resumes a particular paused process, else returns an error.

            example: 'PMan: >bgstart 1234'
        
        'PMan: >pstat <pid>':
            It prints the following information about a existing process:

                Filenama:
                State:
                Utime:
                Stime:
                Rss:
                voluntary_ctxt_switches:
                nonvoluntary_ctxt_switches:

            Else return an error.

    3. 'Ctrl' + 'C' to quit the program


        

            
