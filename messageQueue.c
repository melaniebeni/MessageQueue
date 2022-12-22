#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <time.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/utsname.h>

// structure of how message will look
struct message_buffer {
     long message_type;
    char message_data[100];
} message;

 const int MSG_SIZE = 128;
 const int MSG_CAPACITY = 2;

int main(int argc, const char *argv[]) {
struct mq_attr attr;// initializing struct

attr.mq_maxmsg = MSG_CAPACITY;
attr.mq_msgsize = MSG_SIZE;
int f2;
// Initialize the message queue.
mqd_t mqd = mq_open("/queue",  O_CREAT | O_RDWR | O_NONBLOCK, 0644, &attr);
if (mqd == (mqd_t) -1)
       {
      printf("Parent:  mq_open error");
      mq_unlink(argv[1]); // unlinks message queue if there is an error
       return 2;
       }
f2 = creat("output.txt", 0644);
// pipe
int p[2];
    char numFiles[10];
    int options[4];
    int i, buffer=0, totalFiles=0, numOptions=0;
    if(argv[1][0]=='-') {numOptions = 1;} // ignoring the - before the options arguement
    for(i=0;i<4;i++) options[i]=0;
    // options
    if(numOptions==1) {
        for (i=0;argv[1][i]!='\0';i++) { // checks what the inputted options are
            if(argv[1][i]=='n')
                options[0]=1;
            if(argv[1][i]=='c')
                options[1]=1;
            if(argv[1][i]=='b')
                options[2]=1;
            if(argv[1][i]=='I')
                options[3]=1;
        }
        for(i=0;argv[2][i]!='\0';i++) { // checks what buffer is
            buffer*=10;
            buffer+=argv[2][i]-48;
        }
        totalFiles = argc-3; // total files
     }
      else { // sets the default options if none are inputted
        options[0]=1;
        options[1]=1;
        options[2]=1;

        for(i=0;argv[1][i]!='\0';i++) {
            buffer*=10;
            buffer+=argv[1][i]-48;
        }
        totalFiles = argc-2;
     }
     pipe(p); // pipe
 // fork
pid_t pid = fork();
if (pid < 0) {
    fprintf(stderr, "Fork Failed");
    return 2;
}
 // parent
else if (pid > 0){
    char buf[buffer];
    unsigned int prio;
    struct timespec timeout = {0, 0};
        int count, counter;
        char* temp = (char *)malloc(buffer * sizeof(char)); // allocating space to read in text file
        char c;
        ssize_t numRead;
        int id = getpid();
        char pid = id + '0';
        int idp = getppid();
        char ppid = idp + '0';
        char cwd[128];  //array holds the current working directory
        getcwd(cwd, sizeof(cwd));
        char hostname[1024]; //holds the hostname
        hostname[1023];
        gethostname(hostname, 1023);
        char j;

     write(f2,"Process ID is:\t", 15);
     write(f2, &pid, sizeof pid);
     write(f2,"\n", 1);
     write(STDOUT_FILENO,"Process ID is:\t", 15);
     write(STDOUT_FILENO, &pid, sizeof pid);  // prints the process ID
     write(STDOUT_FILENO,"\n", 1);

     write(f2,"Parent process ID is:\t", 22);
     write(f2, &ppid, 10);
     write(f2,"\n", 1);
     write(STDOUT_FILENO,"Parent process ID is:\t", 22);
     write(STDOUT_FILENO, &ppid, 10);  //prints the parent process ID
     write(STDOUT_FILENO,"\n", 1);

     write(f2,"Process current working directory is:\t", 38);
     write(STDOUT_FILENO,"Process current working directory is:\t", 38);
    for(int f=0;f<100;f++){
        if(cwd[f]!='\0'){
            j=cwd[f];
            write(f2, &j, 1);   // prints the current working directory
            write(STDOUT_FILENO, &j, 1);
        }
    }
     write(f2,"\n", 1);
     write(STDOUT_FILENO,"\n", 1);

      write(f2,"Hostname is:\t", 13);
       write(STDOUT_FILENO,"Hostname is:\t", 13);
      for(int f=0;f<50;f++){
        if(hostname[f]!='\0'){
            j=hostname[f];
            write(f2, &j, 1);  // prints the hostname
            write(STDOUT_FILENO, &j, 1);
        }
    }
    write(f2,"\n", 1);
    write(STDOUT_FILENO,"\n", 1);

     struct utsname uts;
     uname(&uts);
     write(STDOUT_FILENO,"OS name is:\t", 12);
     write(f2,"OS name is:\t", 12);
     write(STDOUT_FILENO, &uts.sysname, sizeof(uts.sysname));
     for(int f=0;f<50;f++){
        if(uts.sysname[f]!='\0'){
            j=uts.sysname[f];
            write(f2, &j, 1);  // prints the system name
        }
    }
     write(STDOUT_FILENO,"\n", 1);
     write(f2,"\n", 1);

     write(STDOUT_FILENO,"OS release is:\t", 15);
     write(f2,"OS release is:\t", 15);
     write(STDOUT_FILENO, &uts.release, sizeof(uts.release));
     for(int f=0;f<50;f++){
        if(uts.release[f]!='\0'){
            j=uts.release[f];
            write(f2, &j, 1);  // prints the system name
        }
    }    // prints the OS release
     write(f2,"\n", 1);
     write(STDOUT_FILENO,"\n", 1);

     write(STDOUT_FILENO,"OS version is:\t", 15);
      write(f2,"OS version is:\t", 15);
     write(f2, &uts.version, sizeof(uts.version));      // prints the OS version
     for(int f=0;f<50;f++){
        if(uts.sysname[f]!='\0'){
            j=uts.sysname[f];
            write(f2, &j, 1);  // prints the system name
        }
    }
     write(STDOUT_FILENO,"\n", 1);
     write(f2,"\n", 1);

    if(numOptions==1)
             counter = 3;   // options were an argument start reading files from arg 3
         else
             counter = 2;
         for(;counter<argc;counter++) {
            int fd = open(argv[counter], O_RDONLY | O_NONBLOCK);   // opens file
            while (1) {
                numRead = read(fd, temp, buffer);      // reads through each file into temp
                if (numRead == 0){
                    close(p[1]);       // closed file after EOF is reached
                    break;
                }
                if( write(p[1], temp, numRead) != numRead){     // write temp to pipe
                    break;
                }
             }
        }
      wait(NULL);       // waits for child process to be done
         if(numOptions==1)
             counter = 3;
         else
             counter = 2;
         for(count=0;count<totalFiles;count++) { // following is done for every inputted txt file

        if(options[0]==1) {    // if option is n
            for(i=0;argv[counter][i]!='\0';i++) {
                temp[i]=argv[counter][i];
            }
            write(f2,temp,i);
            write(f2,":\tnewline count is\t",19);
            write(STDOUT_FILENO,temp,i);
            write(STDOUT_FILENO,":\tnewline count is\t",19);
            ssize_t numRead2 = mq_timedreceive(mqd, buf, attr.mq_msgsize, &prio, &timeout);    // receives from message queue
            if (numRead2 == -1){
                printf("Parent: mq_read error n \n");
            }
            write(f2, buf, (size_t) numRead2);      // writes value from message queue to output file
            write(f2,"\n",1);
            write(STDOUT_FILENO, buf, (size_t) numRead2);      // writes value from message queue to output file
            write(STDOUT_FILENO,"\n",1);
            }

        if(options[1]==1) {     // if option is c
            for(i=0;argv[counter][i]!='\0';i++) {
                temp[i]=argv[counter][i];
            }
            write(f2,temp,i);
            write(f2,":\tword count is\t",16);
            write(STDOUT_FILENO,temp,i);
            write(STDOUT_FILENO,":\tword count is\t",16);
            ssize_t numRead3 = mq_timedreceive(mqd, buf, attr.mq_msgsize, &prio, &timeout);     // receives from message queue
            if (numRead3 == -1){
                printf("Parent: mq_read error c \n");
            }
            write(f2, buf, (size_t) numRead3);      // writes value from message queue to output file
            write(f2,"\n",1);
            write(STDOUT_FILENO, buf, (size_t) numRead3);      // writes value from message queue to output file
            write(STDOUT_FILENO,"\n",1);
            }

        if(options[2]==1) {     // if option is b
            for(i=0;argv[counter][i]!='\0';i++) {
                temp[i]=argv[counter][i];
            }
            write(f2,temp,i);
            write(f2,":\tcharacter count is\t",21);
            write(STDOUT_FILENO,temp,i);
            write(STDOUT_FILENO,":\tcharacter count is\t",21);
            ssize_t numRead4 = mq_timedreceive(mqd, buf, attr.mq_msgsize, &prio, &timeout);      // receives from message queue 
            if (numRead4 == -1){
                printf("Parent: mq_read error b \n");
            }
            write(f2, buf, (size_t) numRead4);      // writes value from message queue to output file
            write(f2,"\n",1);
            write(STDOUT_FILENO, buf, (size_t) numRead4);      // writes value from message queue to output file
            write(STDOUT_FILENO,"\n",1);
            }

        if(options[3]==1) {         // if option is i
            for(i=0;argv[counter][i]!='\0';i++) {
                temp[i]=argv[counter][i];
            }
            write(f2,temp,i);
            write(f2,":\tmaximum line length is\t",25);
            write(STDOUT_FILENO,temp,i);
            write(STDOUT_FILENO,":\tmaximum line length is\t",25);
            ssize_t numRead5 = mq_timedreceive(mqd, buf, attr.mq_msgsize, &prio, &timeout);    // receives from message queue 
            if (numRead5 == -1){
                 printf("Parent: mq_read error i \n");
            }
            write(f2, buf, (size_t) numRead5);       // writes value from message queue to output file
            write(f2,"\n",1);
            write(STDOUT_FILENO, buf, (size_t) numRead5);       // writes value from message queue to output file
            write(STDOUT_FILENO,"\n",1);
            }
        counter++;
    }

     if (mq_close(mqd) == -1) {     // closes the message queue when done
         printf("Parent: close error");
     if (mq_unlink(argv[1]) == -1) {        // unlinks the message queue
         printf("Parent: mq_unlink error");
         return 3;
     }
     }
       wait(NULL);          // ensures the child is done before the parent terminates
       write(f2,"Parent: Terminating.\n",21);
       write(STDOUT_FILENO,"Parent: Terminating.\n",21);
     }
// child
else if (pid == 0)  {
        int count, j, n, q, c, b, I, length;
        char* temp = (char *)malloc(buffer * sizeof(char));
        struct timespec timeout = {0, 0};
        message.message_type = 1;
        ssize_t  numRead;
        n=0, c=0, b=0, I=0;
        close(p[1]);        // closes write end of the pipe
        for(count=0;count<totalFiles;count++) {
            while (1) {
                numRead = read(p[0], temp, buffer);
                for(i=0;i<buffer;i++) {
                    if(temp[i]=='\n')   n++;        // counts newlines
                    if(temp[i] ==' ' && temp[i] !='\n') c++;    // word count
                    if(temp[i]!=' ' && temp[i]!='\n' && temp[i]!=0)  b++;  // character count
                }
                length=0;
                for(i=0;i<buffer;i++) {
                    length++;
                    if(temp[i]=='\n') { //finding the longest line length
                        if(I<length){
                         I=length;
                    }
                    length=0;
                }
            }
            if (numRead == 0) {
                break;
            }
        }
        // the code above produces the values below
            printf("newline: %d\n", n);
            printf("word count: %d\n", c);
            printf("char count: %d\n", b);
            printf("max line length: %d\n", I);
         for(j=0;j<11;j++)
                 message.message_data[j]=' ';  // clears the struct array
             j=10;
             if(options[0]==1) {
                 while(n>0) {
                     message.message_data[j]=(n%10)+48;     // sends the counted value character by character to the parent process to receive
                     n= n/10;
                     j--;
                 }
        // message queue send
         if (mq_timedsend(mqd, message.message_data, sizeof(message.message_data), 1, &timeout)     == -1) {
         printf("Child: mq_send error n \n");
         }
             }
                 for(j=0;j<11;j++)
                 message.message_data[j]=' ';   // clears the struct array
              j=10;
              if(options[1]==1) {
                  while(c>0) {
                      message.message_data[j]=(c%10)+48;    // sends the counted value character by character to the parent process to receive
                      c= c/10;
                      j--;
                  }
        // message queue send
        if (mq_timedsend(mqd, message.message_data, sizeof(message.message_data), 1, &timeout) == -1) {
            printf("Child: mq_send error c \n");
          }
              }
         for(j=0;j<11;j++)
                 message.message_data[j]=' ';       // clears the struct array
             j=10;
             if(options[2]==1) {
                 while(b>0) {
                     message.message_data[j]=(b%10)+48;     // sends the counted value character by character to the parent process to receive
                     b= b/10;
                     j--;
                 }
         // message queue send
         if (mq_timedsend(mqd, message.message_data, sizeof(message.message_data), 1,  &timeout)     == -1) {
            printf("Child: mq_send error b \n");
         }
                      }

             for(j=0;j<11;j++)
                 message.message_data[j]=' ';       // clears the struct array
             if(options[3]==1) {
                 while(I>0) {
                     message.message_data[j]=(I%10)+48;     // sends the counted value character by character to the parent process to receive
                     I= I/10;
                     j--;
                 }
        // message queue send
         if (mq_timedsend(mqd,  message.message_data , sizeof(message.message_data), 1,  &timeout) == -1) {
         printf("Child: mq_send error i \n");
         }
             }

          }
          if (close(p[0]) == -1) {      // close read end of the pipe
                    printf("pipe failed to close");
                }
         free(temp);
         if (mq_close(mqd) == -1) {     // closing m queue
         printf("Child: close error");
        if (mq_unlink(argv[1]) == -1) {     // unlinking child queue
         printf("Child: mq_unlink error");
         return 3;
         }
         }
        write(f2,"Child: Terminating.\n",20);
        write(STDOUT_FILENO,"Child: Terminating.\n",20);
     }  // End of Child branch
    return 0;
     }
 // End of main()