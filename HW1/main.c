#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SIZE 128

void printStudentNameAndGrade(char *name,int fd){

    char buffer[SIZE*16],
    tempBuffer[SIZE*16],
    *result; //For hiding names
    ssize_t bytes_read;
    int enterCount = 0,
        foundFlag=0;

    while ((bytes_read = read(fd, buffer, SIZE*16)) > 0) {
        if (bytes_read == -1){

            perror("Read");
            break;
        }
        strcpy(tempBuffer,buffer);
        for(int i=0;i<bytes_read;i++){
            
            if (buffer[i]=='\n' && enterCount==0){
                
                result = strtok(tempBuffer,",");
                if (!strcmp(result,name)){
                    
                    printf("Student Name: %s ",result);
                    foundFlag=1;
                }    
                result = strtok(NULL,"\n");
                if (foundFlag){
                    printf("Student Grade:%s\n",result);
                    break;
                }
                enterCount++;
            }
            else if (buffer[i]=='\n'){
                result = strtok(NULL,",");
                if (!strcmp(result,name)){
                    
                    printf("Student Name: %s ",result);
                    foundFlag=1;
                } 
                result = strtok(NULL,"\n");
                if (foundFlag){
                    printf("Student Grade:%s\n",result);
                    break;
                }
            }
        }
    }
}

int findStudentName(char *name,int fd){

    char buffer[SIZE*16],
    tempBuffer[SIZE*16],
    *result; //For hiding names
    ssize_t bytes_read;
    int enterCount = 0;

    while ((bytes_read = read(fd, buffer, SIZE*16)) > 0) {
        strcpy(tempBuffer,buffer);
        for(int i=0;i<bytes_read;i++){
            
            if (buffer[i]=='\n' && enterCount==0){
                result = strtok(tempBuffer,",");
                if (!strcmp(result,name)){
                    
                    return 1;
                }    
                result = strtok(NULL,"\n");
                enterCount++;
            }
            else if (buffer[i]=='\n'){
                result = strtok(NULL,",");
                if (!strcmp(result,name)){
                    
                    return 1;
                } 
                result = strtok(NULL,"\n");
            }
        }
        
    }
    return 0;
}

int changeStudentNameAndGrade(char *name,char *grade,int fd){

    char buffer[SIZE*16],
    tempBuffer[SIZE*16],
    *result; //For hiding names
    ssize_t bytes_read;
    off_t current_pos = 0;
    int enterCount = 0;

    while ((bytes_read = read(fd, buffer, SIZE*16)) > 0) {
        strcpy(tempBuffer,buffer);
        for(int i=0;i<bytes_read;i++){
            
            if (buffer[i]=='\n' && enterCount==0){
                result = strtok(tempBuffer,",");
                if (!strcmp(result,name)){
                    
                    lseek(fd, current_pos-2, SEEK_SET);
                    if (write(fd, grade, strlen(grade)) == -1) {
                        perror("Error writing to file");
                        close(fd);
                        return 1;
                    }
                    else{

                        return 0;
                    }
                }    
                result = strtok(NULL,"\n");
                enterCount++;
            }
            else if (buffer[i]=='\n'){
                result = strtok(NULL,",");
                if (!strcmp(result,name)){
                    
                    lseek(fd, current_pos-2, SEEK_SET);
                    if (write(fd, grade, strlen(grade)) == -1) {
                        perror("Error writing to file");
                        close(fd);
                        return 1;
                    }
                    else{

                        return 0;
                    }
                } 
                result = strtok(NULL,"\n");
            }
            current_pos++;
        }
        
    }
    return 0;
}

int compareAsc(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// Function to compare two strings for descending order
int compareDesc(const void *a, const void *b) {
    return strcmp(*(const char **)b, *(const char **)a);
}

// Function to sort student grades by name or grade
void sortStudentGrades(char *fileName, int sortBy, int sortOrder) {
    char **studentGrades;
    int numStudents = 0;

    // Open the file
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Count the number of students
    char buffer[SIZE];
    while (fgets(buffer, SIZE, file) != NULL) {
        numStudents++;
    }
    rewind(file);

    // Allocate memory for student grades
    studentGrades = (char **)malloc(numStudents * sizeof(char *));
    if (studentGrades == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    // Read student grades into memory
    for (int i = 0; i < numStudents; i++) {
        fgets(buffer, SIZE, file);
        studentGrades[i] = strdup(buffer);
    }

    // Close the file
    fclose(file);

    // Sort student grades based on the sorting criteria
    if (sortBy == 1) { // Sort by name
        if (sortOrder == 1) { // Ascending order
            qsort(studentGrades, numStudents, sizeof(char *), compareAsc);
        } else { // Descending order
            qsort(studentGrades, numStudents, sizeof(char *), compareDesc);
        }
    } else { // Sort by grade
        // Assuming grades are the last characters before the newline character
        if (sortOrder == 1) { // Ascending order
            qsort(studentGrades, numStudents, sizeof(char *), compareAsc);
        } else { // Descending order
            qsort(studentGrades, numStudents, sizeof(char *), compareDesc);
        }
    }

    // Print sorted student grades
    for (int i = 0; i < numStudents; i++) {
        printf("%s", studentGrades[i]);
        free(studentGrades[i]); // Free memory allocated by strdup
    }

    // Free memory allocated for student grades array
    free(studentGrades);
}

int main(){

    char *buffer=NULL,
        temp[SIZE],
        message[5*SIZE],
        *command=NULL,
        *commands[]={"gtuStudentGrades","addStudentGrade","searchStudent","sortAll","showAll","listGrades","listSome"};
    size_t len = 0;
    int logFile;
    int file;
    logFile = open("logFile.log",O_WRONLY| O_EXCL | O_CREAT | O_APPEND ,0666);
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
                file = open(fileName,O_WRONLY| O_EXCL | O_CREAT | O_APPEND ,0666);
                if (file == -1){

                    perror("Can not created file!");
                    return -1;
                }
                sprintf(message, "%s named file have been created from child!",fileName);
                write(logFile, message, strlen(message));
                sprintf(message, "Child PID: %d ", getpid());
                write(logFile, message, strlen(message));
                close(file);
                exit(0);
            }
            else {
                wait(NULL);
                sprintf(message, "Parent PID: %d \n", getpid());
                write(logFile, message, strlen(message));
                    
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
            pid_t cpid = fork();
            if (cpid == -1){
                perror("Child Can Not Be Created!\n");
                return -1;
            }
            else if (cpid == 0){
                file = open(fileName,O_RDWR| O_EXCL | O_APPEND,0666);
                if (file == -1){

                    perror("File can not found!Please firstly create it with gtuStudentGrades command.");
                    sprintf(message, "%s named file can not found!Please firstly create it with gtuStudentGrades command.",fileName);
                    write(logFile, message, strlen(message));
                    sprintf(message, "Child PID: %d ", getpid());
                    write(logFile, message, strlen(message));
                    exit(1);
                }
                else {
                    int nameExistanceControl = findStudentName(name,file);
                    if (nameExistanceControl) {
                        close(file); // Close the file before reopening it
                    
                        // Reopen the file with appropriate flags
                        file = open(fileName, O_RDWR, 0666);
                        if (file == -1) {
                            perror("Failed to reopen file");
                            sprintf(message, "Error: Failed to reopen %s", fileName);
                            write(logFile, message, strlen(message));
                            exit(1);
                        }
                    
                        // Successfully reopened the file, now update the grade
                        if (changeStudentNameAndGrade(name, grade, file) == 0) {
                            sprintf(message, "%s named student already exists in %s named file. Student's grade has been updated from child!", name, fileName);
                            write(logFile, message, strlen(message));
                            sprintf(message, "Child PID: %d ", getpid());
                            write(logFile, message, strlen(message));
                            exit(0);
                        } 
                        else {
                            // Handle error when updating grade
                            sprintf(message, "Error: Failed to update grade for %s in %s", name, fileName);
                            write(logFile, message, strlen(message));
                            exit(1);
                        }
                    }
                    else {
                        sprintf(message,"%s, %s\n",name,grade);
                        write(file,message, strlen(message));
                        sprintf(message, "%s named file have been found.Student %s with his %s grade added to file from child!",fileName,name,grade);
                        write(logFile, message, strlen(message));
                        sprintf(message, "Child PID: %d ", getpid());
                        write(logFile, message, strlen(message));
                        close(file);
                        exit(0);
                    }
                }
            }
            else {
                wait(NULL);
                sprintf(message, "Parent PID: %d \n", getpid());
                write(logFile, message, strlen(message));
                    
            }         
        }
        else if (!strcmp(command,commands[2]))
        {
            char *restOfString,
                fileName[SIZE],
                name[SIZE];
            restOfString = index(buffer,'"');
            strcpy(temp,&restOfString[1]);
            strtok(temp,"\"");
            strcpy(name,temp);     
            strcpy(temp,restOfString); 
            strtok(temp,"\"");
            strcpy(temp,strtok(NULL," \""));
            strcpy(fileName,temp);  
            pid_t cpid = fork();
            if (cpid == -1){
                perror("Child Can Not Be Created!\n");
                return -1;
            }
            else if (cpid == 0){
                file = open(fileName,O_RDONLY| O_EXCL | O_APPEND,0666);
                if (file == -1) {
                            perror("Failed to reopen file");
                            sprintf(message, "Error: Failed to reopen %s", fileName);
                            write(logFile, message, strlen(message));
                            exit(1);
                }
                else {
                    if(findStudentName(name,file)){
                        if (lseek(file, 0, SEEK_SET) == -1) {
                            close(file);
                            return 1;
                        }
                        printStudentNameAndGrade(name,file);
                        sprintf(message, "%s named file have been found.%s named student found and printed its name and grade to terminal!",fileName,name);
                        write(logFile, message, strlen(message));
                        sprintf(message, "Child PID: %d ", getpid());
                        write(logFile, message, strlen(message));
                        exit(0);
                    }
                    else {
                        printf("Student could not founded in %s file\n",fileName);
                        sprintf(message, "%s named file have been found.But %s named student could not found!",fileName,name);
                        write(logFile, message, strlen(message));
                        sprintf(message, "Child PID: %d ", getpid());
                        write(logFile, message, strlen(message));
                        close(file);
                        exit(0);

                    }
                }
                  
            }
            else {
                wait(NULL);
                sprintf(message, "Parent PID: %d \n", getpid());
                write(logFile, message, strlen(message));
                
            }
              
        }
        else if (!strcmp(command,commands[3]))
        {
            char *restOfString,
                fileName[SIZE],
                order[20];
            int choice1,
                choice2;    
            restOfString = index(buffer,'"');
            strcpy(temp,&restOfString[1]);
            strtok(temp,"\"");
            strcpy(fileName,temp);      
            do{
                printf("For order by name please press 1, for order by grades please press 2!\n");
                scanf ("%d",&choice1);
            }while (choice1!=1 && choice1!=2);
            do{
                printf("For order by ascending order please press 1, for order by descending order please press 2!\n");
                scanf ("%d",&choice2);
            }while (choice2!=1 && choice2!=2);
            sortStudentGrades(fileName,choice1,choice2);
                
        }
        else if (!strcmp(command,commands[4]))
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
                file = open(fileName,O_RDONLY| O_EXCL | O_APPEND,0666);
                if (file == -1) {
                    perror("Failed to open file");
                    sprintf(message, "Error: Failed to open %s", fileName);
                    write(logFile, message, strlen(message));
                    exit(1);
                }
                else {
                    char buffer[SIZE * 16];
                    ssize_t bytes_read;
        
                    while ((bytes_read = read(file, buffer, SIZE * 16)) > 0) {
                        // Null-terminate the buffer
                        buffer[bytes_read] = '\0';
                        printf("%s\n", buffer);
                    }
        
                    if (bytes_read == -1) {
                        perror("Read error");
                        sprintf(message, "File %s could not readed and can not printed to terminal!", fileName);
                    }
                    else {

                        sprintf(message, "File %s succesfully printed to terminal!", fileName);
                        write(logFile, message, strlen(message));
                        sprintf(message, "Child PID: %d ", getpid());
                        write(logFile, message, strlen(message));
                        close(file);
                        exit(0); 
                    }

                }
                 
            }
            else {
                wait(NULL);
                sprintf(message, "Parent PID: %d \n", getpid());
                write(logFile, message, strlen(message));
                    
            } 
        }
        else if (!strcmp(command,commands[5]))
        {
            int enterCount=0;
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
                file = open(fileName,O_RDONLY| O_EXCL | O_APPEND,0666);
                if (file == -1) {
                    perror("Failed to open file");
                    sprintf(message, "Error: Failed to open %s", fileName);
                    write(logFile, message, strlen(message));
                    exit(1);
                }
                else {
                    char buffer[SIZE * 16];
                    ssize_t bytes_read;
        
                    while ((bytes_read = read(file, buffer, SIZE * 16)) > 0) {
                        // Null-terminate the buffer
                        for(int i=0;i<bytes_read;i++){
                            printf("%c",buffer[i]);
                            if (buffer[i]=='\n'){
                                enterCount++;
                                
                            }
                            if (enterCount==5){
                                break;
                            }
                        }
                    }
        
                    if (bytes_read == -1) {
                        perror("Read error");
                        sprintf(message, "File %s could not readed and can not printed to terminal!", fileName);
                    }
                    else {

                        sprintf(message, "File %s's first five entries succesfully printed to terminal!", fileName);
                        write(logFile, message, strlen(message));
                        sprintf(message, "Child PID: %d ", getpid());
                        write(logFile, message, strlen(message));
                        close(file);
                        exit(0); 
                    }

                }
                 
            }
            else {
                wait(NULL);
                sprintf(message, "Parent PID: %d \n", getpid());
                write(logFile, message, strlen(message));
                    
            } 
        }
        else if (!strcmp(command,commands[6]))
        {
            int enterCount=0;
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
                file = open(fileName,O_RDONLY| O_EXCL | O_APPEND,0666);
                if (file == -1) {
                    perror("Failed to open file");
                    sprintf(message, "Error: Failed to open %s", fileName);
                    write(logFile, message, strlen(message));
                    exit(1);
                }
                else {
                    char buffer[SIZE * 16];
                    ssize_t bytes_read;
        
                    while ((bytes_read = read(file, buffer, SIZE * 16)) > 0) {
                        // Null-terminate the buffer
                        for(int i=0;i<bytes_read;i++){
                            printf("%c",buffer[i]);
                            if (buffer[i]=='\n'){
                                enterCount++;
                                
                            }
                            if (enterCount==5){
                                break;
                            }
                        }
                    }
        
                    if (bytes_read == -1) {
                        perror("Read error");
                        sprintf(message, "File %s could not readed and can not printed to terminal!", fileName);
                    }
                    else {

                        sprintf(message, "File %s's first five entries succesfully printed to terminal!", fileName);
                        write(logFile, message, strlen(message));
                        sprintf(message, "Child PID: %d ", getpid());
                        write(logFile, message, strlen(message));
                        close(file);
                        exit(0); 
                    }

                }
                 
            }
            else {
                wait(NULL);
                sprintf(message, "Parent PID: %d \n", getpid());
                write(logFile, message, strlen(message));
                    
            } 
        }
        else if (strcmp(command, "q\n") == 0) {
            break;
        } 
        else{
            printf("Wrong command please try again!\n");
        }
        //fseek(stdin, 0, SEEK_END);
    }
    while (1);


    free(buffer);
    close(logFile);
    return 0;
}