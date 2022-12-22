# MessageQueue
Uses the fork() system call to create a child process. The parent and child utilize only two IPC
resources between them: one POSIX message queue and one ordinary pipe.  

./filename.exe [OPTION]... BUFFER_SIZE [INPUT FILE]...

The OPTIONS are the following. If none are specified on the command line then -ncb is used by default.

-n   prints the newline count for each input 
-c   prints the word counts for each input
-b   prints the character counts for each input
-I   prints the maximum line length for each input

BUFFER_SIZE is a number between 32 and 256 inclusively and must be specified. 
[INPUT FILE] contains a list of input files. If no input file is specified, then myfile.txt is used. 

Uses the POSIX system file I/O and process management system calls as well as those for POSIX message queues and ordinary pipes. 

Doesn't use any C standard I/O library functions nor string library functions.

Only the parent process reads the input as given by [INPUT FILE]. The parent writes the input file contents in chunks as indicated by BUFFER_SIZE to the pipe. The child reads this content from the pipe in chunks as indicated by BUFFER_SIZE.

The child process computes the counts as indicated by [OPTION] for each input file separately from any other input file.

The child process sends each count (as indicated by [OPTION]) for each input file as its own POSIX message into the POSIX message
   queue. Therefore, as an example, if -lwmL is indicated with three input files on the command line, the child process sends at least
   4 * 3 = 12 messages into the POSIX message queue.

The parent process receives the messages from (6) above in the POSIX message queue and writes the following to standard output
   in the following format according to each [OPTION] requested for each input file [INPUT FILE] where NEWLINE_COUNT indicates the newline
   count, WORD_COUNT is the word count, CHAR_COUNT is the character count, and MAX_LINE is the maximum line length.
   
Also prints the following

1. pid of the process     

2. pid of the parent process

3. current working directory of the process

4. hostname of the process

5. OS name     

6. OS release

7. OS version
