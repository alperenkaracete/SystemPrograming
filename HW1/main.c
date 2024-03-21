#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SIZE 128

int main(){

    char *buffer=NULL,
        temp[SIZE],
        message[SIZE],
        *command=NULL,
        *commands[]={"gtuStudentGrades","addStudentGrade","searchStudent","sortAll","Display Student Grades"};
    size_t len = 0;
    int logFile;
    int file;
    logFile = open("logFile.log",O_WRONLY| O_EXCL | O_CREAT | O_APPEND ,S_IRUSR);
    if (logFile == -1){

        perror("Can Not Create Log File Exiting!");
        return -1;
    }
    pid_t cpid = fork();
    if (cpid == -1){
        perror("Child Can Not Be Created!\n");
        return -1;
    }
    else if (cpid == 0){
        strcpy(message, "Log file created from child!");
        write(logFile, message, strlen(message));
        sprintf(message, "Child PID: %d ", getpid());
        write(logFile, message, strlen(message));
        exit(0);
    }
    else {
        wait(NULL);
        sprintf(message, "Parent PID: %d \n", getpid());
        write(logFile, message, strlen(message));
    }
    do{
        printf("Please enter commands,for exit please press 'q'\n");
        getline(&buffer,&len,stdin);
        strcpy(temp,buffer);
        command = strtok(temp," ");
        if (!strcmp(command,commands[0]))
        {
            char *restOfString,
                fileName[SIZE];
            restOfString = index(buffer,'"');
            strcpy(temp,&restOfString[1]);
            strtok(temp,"\"");
            strcpy(fileName,temp);
            pid_t cpid = fork();
            if (cpid == -1){
                perror("Child Can Not Be Created!\n");
                return -1;
            }
            else if (cpid == 0){
                file = open(fileName,O_WRONLY| O_EXCL | O_CREAT | O_APPEND ,S_IRUSR);
                if (file == -1){

                    perror("Can not created file!");
                    return -1;
                }
                strcpy(message, "File created from child!");
                write(logFile, message, strlen(message));
                sprintf(message, "Child PID: %d ", getpid());
                write(logFile, message, strlen(message));
                exit(0);
            }
            else {
                wait(NULL);
                sprintf(message, "Parent PID: %d \n", getpid());
                write(logFile, message, strlen(message));
                if (file !=-1)
                    close(file);
            }
        }
        else if (!strcmp(command,commands[1]))
        {
            char *restOfString,
                name[SIZE],
                grade[3],
                fileName[SIZE],
                *result;
                
            restOfString = index(buffer,'\"');
            strcpy(temp,&restOfString[1]);
            strtok(temp,"\"");
            strcpy(name,temp); 
            strcpy(temp,restOfString); 
            strtok(temp,"\"");
            strcpy(temp,strtok(NULL," \""));
            strcpy(grade,temp); 
            strcpy(temp,restOfString);
            result = strtok(temp,"\"");
            while (result[0]!='\n'){
                result = strtok(NULL,"\"");
                if (result!=NULL && result[0] != '\n' && result[0] != ' ')
                    strcpy(fileName,result);
            }
            
            
            
        }
        else if (!strcmp(command,commands[2]))
        {
            char *restOfString,
                name[SIZE];
            restOfString = index(buffer,'"');
            strcpy(temp,&restOfString[1]);
            strtok(temp,"\"");
            strcpy(name,temp);             
        }
        else if (!strcmp(command,commands[3]))
        {
            char *restOfString,
                fileName[SIZE],
                order[20];
            restOfString = index(buffer,'"');
            strcpy(temp,&restOfString[1]);
            strtok(temp,"\"");
            strcpy(fileName,temp);    
            strtok(restOfString,"\"");
            restOfString = strtok(NULL," ");
            strcpy(order,restOfString);  
            order[strlen(order)-1] = '\0';     
        }
        else if (!strcmp(command,commands[4]))
        {
            
        }
        else if (command[0]!='q' && command[1]!='\0'){

            printf("Wrong command please try again!\n");
        }
    }
    while (command[0]!='q' && command[1]!='\0');


    free(buffer);
    close(logFile);
    return 0;
}