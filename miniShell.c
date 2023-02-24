#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
//declaration functions:
int countWords(char []);
void placeOf(char [] , char**, int);
void readFromFile(FILE*);
void freeM(char**,int);
void  pipeFun1(char []);
void  pipeFun2(char []);
void removeLastBackSpace(char []);
void removeFrontBackSpace(char []);
void removeInvertedCommas(char []);
void writeToFile(char [],FILE*);
void changeFromNum(char [],int);
void ampersand(char[],pid_t,pid_t[],int,int*);
void sig_handler(int);
void noHup(char [],pid_t);
int main() {
    char str[512], cwd[512];
    FILE *fp;
    pid_t x;
    pid_t waitReturnPid[512];
    int s = 0;
    int words, countCommands = 0, sumWords = 0, countLines = 0, num1 = 0, num = 0,p = 0,status;
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) //get the current position
            printf("%s>", getcwd(cwd, sizeof(cwd)));
        fgets(str, 512, stdin); //input from user
        str[strlen(str) - 1] = '\0';
//error with input command
        if (str[0] == ' ' || str[strlen(str) - 1] == ' ') {
            p = countWords(str);
            if(p < 1)
                continue;
            else
                fprintf(stderr, "error with the given command,try again\n");
            continue;}

// if the string command '!' plus pipe
        if(strchr(str,'|') && strchr(str,'!')){
            int c = 0;
            for(int i = 0; str[i] != '\0'; i++)
                if(str[i] == '|')
                    c++;
            if(c > 2){
                fprintf(stderr, "too much pipes");
                continue;}
            changeFromNum(str,c);}

// if the string command '!' plus number
        if (str[0] == '!' && str[1] != ' ') {
            char str1[512];
            fp = fopen("file.txt", "r"); //open the file for reading only
            if (fp == NULL) {     // error opening the file
                fprintf(stderr, "\nUnable to open file!\n");
                exit(1);}
            strcpy(str1, "");
            strcpy(str1, str + 1); //save the number in str1 arrayy
            num1 = atoi(str1); //function change the string to number
            str1[0] = '\0';
            while (fgets(str1, 512, fp) != NULL) { //read from file line by line
                if (num1 == 1)
                    break;
                str1[0] = '\0';
                num1--;}
            //now str1 contain the command that we want
            str1[strlen(str1)-1] = '\0';
            strcpy(str, str1+1);
            str[strlen(str1) - 1] = '\0';
            fclose(fp);} //closing the file
//if the number from '!' command is not in history
        if (num1 > 1) {
            printf("NOT IN HISTORY");
            num1 = 0;
            continue;}

//the string is 'nohup' command
        if(strncmp(str,"nohup",5) == 0 && str[5] == ' '){
            countCommands++;
            sumWords += countWords(str);
            removeLastBackSpace(str);
            writeToFile(str,fp);
            writeToFile("\n",fp);
            noHup(str,x);
            continue;}

//the string contain '&' in the end
        if(strchr(str,'&')){
            countCommands++;
            removeLastBackSpace(str);
            sumWords += countWords(str);
        if(str[strlen(str)-1] == '&'){
            writeToFile(str,fp);
            writeToFile("\n",fp);
            ampersand(str,x,waitReturnPid,words,&s);
            continue;}}

// str contain '||' command -edge case
        if(strstr(str,"||")){
            int i,flag = 0;
            for(i = 0 ;str[i+2] != '\0'; i++){
                if(str[i] == '|' && str[i+1] == '|' && str[i+2] == '|'){
                    flag = 1;
                    break;}
                if(str[i] == '|' && str[i+1] == '|')
                    break;}
            if(flag == 1){
                    fprintf(stderr, "syntax error");
                    continue;}
            countCommands++;
            sumWords += countWords(str);
            writeToFile(str,fp);
            writeToFile("\n",fp);
            char *str1 = (char *) malloc(((i+1) * sizeof(char) ));
            if (str1 == NULL) { //the dynamic memory not success
                perror("malloc failed");
                exit(1);}
            strncpy(str1,str,i);
            str1[i] = '\0';
            str[0] = '\0';
            strcpy(str,str1);
            free(str1);}

//input contain pipe command
        if(strchr(str,'|') && (!(strstr(str,"||")))) {
            int c = 0, i;
            for (i = 0; str[i] != '\0'; i++)
                if (str[i] == '|' && str[i - 1] != '|' && str[i + 1] != '|')
                    c++;
            if (c > 2) {//more than 2 pipes
                fprintf(stderr, "too much pipes!");
                continue;}
            countCommands++;
            sumWords += countWords(str);
            writeToFile(str,fp);
            writeToFile("\n",fp);
            removeInvertedCommas(str);//delete inverted from string
            if(c == 1)
                 pipeFun1(str);
            if(c == 2)
                pipeFun2(str);
            continue;
        }
        words = countWords(str);
        if (words < 1) //put 'enter' key or backspaces only
            continue;
        countCommands++;
        sumWords += words;
        char **command = (char **) malloc(((words + 1) * sizeof(char *) )); // dynamic array of the number of words
        if (command == NULL) { //the dynamic memory not success
            perror("malloc failed");
            exit(1);}
        command[words] = NULL; // put 'NULL' in last spot, after we can send the array to execvp
        placeOf(str, command, words); // send the sentence to function

//if the string is words 'cd'
        if (strcmp(command[0], "cd") == 0 && command[1] == NULL && words == 1 && str[0] != ' ' && str[2] != ' ') {
            printf(("command not supported (Yet)\n"));
            freeM(command,words); //free the dynamic memory
            continue;}
        countLines++;

        fp = fopen("file.txt", "a"); //open the file to append
        if (fp == NULL) { // error opening the file
            fprintf(stderr, "\nUnable to open file!\n");
            exit(1);}
        fprintf(fp, " %s\n", str); //append sentence to the file
        fclose(fp);
//if the string is words 'history'
        if (strcmp(str, "history") == 0 || strcmp(command[0], "history") == 0) { // checks "history"
            readFromFile(fp);
            freeM(command,words); //free the dynamic memory
            continue;}
//if the string is the word 'done'
        if ((strcmp(command[0], "done") == 0 && command[1] == NULL && words == 1 && str[0] != ' ' && str[4] != ' ') ||
            strcmp(command[0], "done") == 0 && command[1] == NULL) {
            printf("Num of commands: %d\n", countCommands);
            printf("Total number of words in all commands: %d !\n", sumWords-1);
            freeM(command,words);//free the dynamic memory
            for(int j = 0; j < s; j++) //send for all the 'sleep&' processes signal to finish
                kill(waitReturnPid[j],SIGKILL);
            exit(0);}
        x = fork();  //duplicate the process "father" and "son"
        if (x < 0) { // error with the fork
            perror("error with the fork()");
            freeM(command,words);
            exit(1);}
        if (x == 0) {  //child process
            if(execvp(command[0], command) == -1)
                perror("error with execvp()"); //if execvp failed we enter here
            freeM(command,words);
            exit(1);
        } else {  //father process
            waitpid(x,NULL,0); //father is waiting for his son process
            freeM(command,words); //free the dynamic memory
            continue;
        }}}

int countWords(char str[]) { //function check how many words in string
    int i, c = 0, w = 1;
    for (i = 0; str[i] != '\0'; i++) { // loop 'for'- check each word one by one
        if (str[i] != ' ') // check if the word is not backspace
            c++;

        if (c > 0 && str[i] == ' ') { // if the word is backspace and its not the first word
            if (str[i - 1] != ' ' && str[i + 1] != '\0' && str[i + 1] !=' ') { // the current word is backspace and before+after there is word different from backspace
                w++;
                continue;}

            if (str[i - 1] == ' ' && str[i + 1] != '\0' && str[i + 1] != ' ') { // before the current word there is space and after there isnt backspace
                w++;
                continue;}
        }
        if(str[i] == '|'){
            if(str[i-1] != '|' && str[i-1] != ' ' && str[i+1] != '|' && str[i+1] != ' '){
            w++;}
            if(str[i-1] == ' ' && str[i+1] == ' '){
                w--;}
        }}
        if (c == 0) // if there are no words
            w = 0;
        return w;
    }
//make array of the words from the given sentence
    void placeOf(char str[], char **command, int words) {
        int i, c = 0, j = 0;
            for (i = 0; str[i] != '\0'; i++) { //check each char
                if (str[i] != ' ' && str[i] != '|' && str[i + 1] != '\0')
                    c++;

                if (str[i] == ' ' && words > 1 && i != 0) { // if the word is backspace and its not the first word
                    if (str[i - 1] != ' ' && str[i + 1] != '\0' && str[i + 1] != ' ') {
                        command[j] = (char *) malloc(sizeof(char) * c  + 1);//make dynamic memory
                        if (command[j] == NULL) {
                            perror("malloc failed");
                            exit(1);
                        }
                        command[j][0] = '\0';
                        strncpy(command[j], str + (i - c), c);//copy the string from thw wanted index
                        command[j][c] = '\0';
                        j++;
                        words--;
                        c = 0;
                        continue;
                    }
                    if (i - 1 >= 0 && str[i - 1] != ' ' && str[i + 1] != '\0' && str[i + 1] == ' ') {
                        command[j] = (char *) malloc(sizeof(char) * c + 1);//make dynamic memory
                        if (command[j] == NULL) {
                            perror("malloc failed");
                            exit(1);
                        }
                        command[j][0] = '\0';
                        strncpy(command[j], str + (i - c), c);//copy the string from thw wanted index
                        command[j][c] = '\0';
                        j++;
                        words--;
                        c = 0;
                        continue;
                    } else
                        continue;
                }
                if (words <= 1 && str[i] != ' ') {//  words ==1
                    command[j] = (char *) malloc(sizeof(char) * (strlen(str) - i) + 1);//make dynamic memory
                    if (command[j] == NULL) {
                        perror("malloc failed");
                        exit(1);
                    }
                    command[j][0] = '\0';
                    strncpy(command[j], str + i, strlen(str) - i);//copy the string from thw wanted index
                    command[j][strlen(str) - i] = '\0';
                    if(command[j][strlen(command[j])-1] == ' ')
                        removeLastBackSpace(command[j]);
                    break;
                }
            }
        }

//function reading from the file
    void readFromFile(FILE *fp) {
        char s[520];
        fp = fopen("file.txt", "r"); // opening the file for reading only
        if (fp == NULL) { // error opening the file
            fprintf(stderr, "\nUnable to open file!\n");
            exit(1);}
        int m = 1;
        while (fgets(s, 512, fp)) { // read line by line
            printf("%d %s", m, s); // Print each line
            m++;
        }
        fclose(fp);
    }
//function writing to the file
void writeToFile(char str[],FILE *fp) {
    fp = fopen("file.txt", "a"); //open the file to append
    if (fp == NULL) { // error opening the file
        fprintf(stderr, "\nUnable to open file!\n");
        exit(1);}
    fprintf(fp, " %s", str); //append sentence to the file
    fclose(fp);
}
//function free the dynamic memory
    void freeM(char **command, int w) {
        int i;
        for (i = 0; i <= w; i++) {
            free(command[i]);//free each of the dynamic array
            command[i] = NULL;}
        free(command); //free the main memory
        command = NULL;}
//only one pipe function
    void pipeFun1(char str[]) {
        int i, word1, word2;
        char *str1,*str2;
        FILE *fp;
        for (i = 0; str[i] != '\0'; i++) {
            if (str[i] == '|' && str[i - 1] != '|' && str[i + 1] != '|')
                break;}
        str1 = (char*) malloc(sizeof(char) * i + 1);
        if (str1 == NULL) {
            perror("malloc failed");
            exit(1);}
        str1[0] = '\0';
        strncpy(str1,str,i);
        str1[i] = '\0';
        word1 = countWords(str1);
        char **command = (char **) malloc(((word1 + 1) * sizeof(char *)));
        if (command == NULL) {
            perror("malloc failed");
            exit(1);
        }
        command[word1] = NULL; // put 'NULL' in last spot, after we can send the array to execvp
        placeOf(str1, command, word1); // send the sentence to function
        str2 = (char*) malloc(sizeof(char) * (strlen(str) - i) + 1);
        if (str2 == NULL) {
            perror("malloc failed");
            exit(1);}
        str2[0] = '\0';
        strncpy(str2, str + i + 1, strlen(str) - i);
        str2[strlen(str) - i] = '\0';
        word2 = countWords(str2);
        char **command1 = (char **) malloc(((word2 + 1) * sizeof(char *)));
        if (command1 == NULL) {
            perror("malloc failed");
            exit(1);}
        command1[word2] = NULL; // put 'NULL' in last spot, after we can send the array to execvp
        placeOf(str2, command1, word2); // send the sentence to function
        int pipe_f[2];
        if (pipe(pipe_f) == -1) {//creating the pipe
            perror("pipe failed");
            exit(1);}
        for(int k=0; k<word1 ; k++) //dismantle each word to the history
                writeToFile(command[k],fp);
            writeToFile("\n",fp);
            for(int k=0; k<word2 ; k++)//dismantle each word to the history
                writeToFile(command1[k],fp);
        writeToFile("\n",fp);
        int status;
        pid_t x;
        pid_t x1;
        if ((x = fork()) == 0) { //forking the first child,now in first child process
            close(pipe_f[0]);
            if (dup2(pipe_f[1], STDOUT_FILENO) == -1){//change the output to the pipe
                perror("error with the pipe");
                exit(1);}
            close(pipe_f[1]);
            execvp(command[0], command);
            perror("error with the execvp");//if we go here, error with the execvp
            free(str1);
            free(str2);
            freeM(command, word1);
            freeM(command1, word2);
            exit(1);}
        else if ((x1 = fork()) == 0) {//forking the second child,now in the child process
            close(pipe_f[1]);
            if (dup2(pipe_f[0], STDIN_FILENO) == -1){//reading from the pipe
                perror("error with the pipe");
                exit(1);}
            close(pipe_f[0]);
            execvp(command1[0], command1);
            perror("error with the execvp");//if we go here, error with the execvp
            free(str1);
            free(str2);
            freeM(command, word1);
            freeM(command1, word2);
            exit(1);}
        else {//the father process
            wait(&status);//waiting for the children process
            close(pipe_f[0]);
            close(pipe_f[1]);
            free(str1);
            free(str2);
            freeM(command, word1);
            freeM(command1, word2);
            return;}}
//two pipe function
        void pipeFun2(char str[]) {
            int i,j, word3,word4;
            char *str3,*str4,*str5;
            FILE *fp;
        for (i = strlen(str) - 1; i >= 0; i--)
                if (str[i] == '|')
                    break;
            str3 = (char*) malloc(sizeof(char) * i + 1);//take the first string till the second pipe
            if (str3 == NULL) {
                perror("malloc failed");
                exit(1);}
            str3[0] = '\0';
            strncpy(str3, str , i);//copy the string from thw wanted index
            str3[i] = '\0';
            str4 = (char*) malloc(sizeof(char)*(strlen(str) - i) + 1);
            if (str4 == NULL) {
                perror("malloc failed");
                exit(1);}
            str4[0] = '\0';
            strncpy(str4, str+i+1 , strlen(str)-i);//copy the string from thw wanted index
            str4[strlen(str)-i] = '\0';
            word3 = countWords(str4);
            char **command2 = (char **) malloc(((word3 + 1) * sizeof(char *)));
            if (command2 == NULL) {
                perror("malloc failed");
                exit(1);}
            command2[word3] = NULL; // put 'NULL' in last spot, after we can send the array to execvp
            placeOf(str4, command2, word3); // send the sentence to function
            int status;
            int pipe_f[2];
            if (pipe(pipe_f) == -1) {//creating the pipe
                perror("pipe failed");
                exit(1);}
            pid_t x;
            pid_t x1;
            if ((x = fork()) == 0) {//forking the first son
                close(pipe_f[0]);
                if (dup2(pipe_f[1], STDOUT_FILENO) == -1){//writing to the pipe
                    perror("error with the pipe");
                    exit(1);}
                close(pipe_f[1]);
                pipeFun1(str3); //using the written function for the first string
                exit(0);}
            else if ((x1 = fork()) == 0) {//forking the second son
                close(pipe_f[1]);
                if (dup2(pipe_f[0], STDIN_FILENO) == -1){//reading from the pipe
                    perror("error with the pipe");
                    exit(1);}
                close(pipe_f[0]);
                execvp(command2[0], command2);
                perror("error with the execvp");//we go here if execvp fail
                free(str3);
                free(str4);
                freeM(command2, word3);
                exit(1);}
            else {//father process
                wait(&status);
                close(pipe_f[0]);
                close(pipe_f[1]);
                free(str3);
                free(str4);
                for(i = 0; i<word3 ; i++)
                    writeToFile(command2[i],fp);
                writeToFile("\n",fp);
                freeM(command2, word3);
                return;}}
//function remove the backspaces from the end of sentence
void removeLastBackSpace(char str[]){
//    int j = 0;
    int k = strlen(str);
    for(int i = k - 1; i>=0 ;i--){
        if(str[i] == ' '){
            str[i] = '\0';}
        else
            break;}}
//function remove the backspaces from the start of sentence
void removeFrontBackSpace(char str[]){
    int i;
    char *str1 = (char*)malloc(sizeof(char) * strlen(str));
    for(i = 0; str[i] != '\0';i++)
        if(str[i] != ' ')
            break;
    strncpy(str1,str+i, strlen(str)-i);
    str1[strlen(str)-i] = '\0';
    str[0] = '\0';
    strcpy(str,str1);
    str[strlen(str)] = '\0';
    free(str1);
}
//function remove the inverted from the sentence
void removeInvertedCommas(char str[]){
    char *str1 = (char*) malloc(sizeof(char) * (strlen(str) ) + 1);
    if (str1 == NULL) {
        perror("malloc failed");
        exit(1);}
    str1[0] = '\0';
    int j = 0;
    for(int i=0; i < strlen(str) ;i++){
        if(str[i] != '"'){
            str1[j] = str[i];
            j++;}}
    str1[j] = '\0';
    strcpy(str,str1);
    free(str1);
}
//function reading from the history the given number
void changeFromNum(char str[],int num) {
    FILE *fp;
    char str1[512], str2[512], str3[512];
    int i, j = 0, num1;
    if (num == 1) { //if there is one pipe
        fp = fopen("file.txt", "r"); //open the file for reading only
        if (fp == NULL) {     // error opening the file
            fprintf(stderr, "\nUnable to open file!\n");
            exit(1);
        }
        for (i = 0; str[i] != '\0'; i++)
            if (str[i] == '|')
                break;
        strncpy(str1, str, i);
        strncpy(str2, str + i + 1, strlen(str) - i);
        str1[i] = '\0';
        str2[strlen(str) - i] = '\0';
        removeLastBackSpace(str1);
        removeFrontBackSpace(str2);
        if (str1[0] == '!') {
            num1 = atoi(str1 + 1); //function change the string to number
            str1[0] = '\0';
            while (fgets(str1, 512, fp) != NULL) { //read from file line by line
                if (num1 == 1)
                    break;
                str1[0] = '\0';
                num1--;
            }
            str1[strlen(str1) - 1] = '\0';
            if(num1 > 1){
                printf("NOT IN HISTORY");
                str[0] = '\0';
                return;}}
        fclose(fp);
        fp = fopen("file.txt", "r"); //open the file for reading only
        if (fp == NULL) {     // error opening the file
            fprintf(stderr, "\nUnable to open file!\n");
            exit(1);}
        if (str2[0] == '!') {
            num1 = atoi(str2 + 1); //function change the string to number
            str2[0] = '\0';
            while (fgets(str2, 512, fp) != NULL) { //read from file line by line
                if (num1 == 1)
                    break;
                str2[0] = '\0';
                num1--;}
            str2[strlen(str2) - 1] = '\0';
            if(num1 > 1){
                printf("NOT IN HISTORY");
                str[0] = '\0';
                 return;}}
        str[0] = '\0';
        strcpy(str, str1);
        strcat(str, "|");
        strcat(str, str2);
        str[strlen(str)] = '\0';
        fclose(fp);}
    if (num == 2) {//if there is two pipe
        fp = fopen("file.txt", "r"); //open the file for reading only
        if (fp == NULL) {     // error opening the file
            fprintf(stderr, "\nUnable to open file!\n");
            exit(1);
        }
        for (i = 0; str[i] != '\0'; i++)
            if (str[i] == '|')
                break;
        j = i;
        strncpy(str1, str, i);
        str1[i] = '\0';
        for (i = strlen(str) -1 ; i >= 0; i--)
            if (str[i] == '|')
                break;
        strncpy(str3, str+i+1, strlen(str)- i);
        str3[strlen(str)- i] = '\0';
        strncpy(str2, str+j+1, i-j-1);
        str2[i-j-1] = '\0';
        removeLastBackSpace(str1);
        removeLastBackSpace(str2);
        removeFrontBackSpace(str2);
        removeFrontBackSpace(str3);
        if(str1[0] == '!'){
            num1 = atoi(str1+1); //function change the string to number
            str1[0] = '\0';
            while (fgets(str1, 512, fp) != NULL) { //read from file line by line
                if (num1 == 1)
                    break;
                str1[0] = '\0';
                num1--;}
            str1[strlen(str1)-1] = '\0';
            if(num1 > 1){
                printf("NOT IN HISTORY");
                str[0] = '\0';
                return;}}
        fclose(fp);
        fp = fopen("file.txt", "r"); //open the file for reading only
        if (fp == NULL) {     // error opening the file
            fprintf(stderr, "\nUnable to open file!\n");
            exit(1);}

        if(str2[0] == '!'){
            num1 = atoi(str2+1); //function change the string to number
            str2[0] = '\0';
            while (fgets(str2, 512, fp) != NULL) { //read from file line by line
                if (num1 == 1)
                    break;
                str2[0] = '\0';
                num1--;}
            str2[strlen(str2)-1] = '\0';
            if(num1 > 1){
                printf("NOT IN HISTORY");
                str[0] = '\0';
                return;}}
        fclose(fp);
        fp = fopen("file.txt", "r"); //open the file for reading only
        if (fp == NULL) {     // error opening the file
            fprintf(stderr, "\nUnable to open file!\n");
            exit(1);}
        if(str3[0] == '!'){
            num1 = atoi(str3+1); //function change the string to number
            str3[0] = '\0';
            while (fgets(str3, 512, fp) != NULL) { //read from file line by line
                if (num1 == 1)
                    break;
                str3[0] = '\0';
                num1--;}
            str3[strlen(str3)-1] = '\0';
            if(num1 > 1){
                printf("NOT IN HISTORY");
                str[0] = '\0';
                return;}}
        str[0] = '\0';
        strcpy(str,str1);
        strcat(str,"|");
        strcat(str,str2);
        strcat(str,"|");
        strcat(str,str3);
        str[strlen(str)] = '\0';
        fclose(fp);}}
//function with ampersand char
void ampersand(char str[],pid_t x,pid_t waitReturnPid[], int words,int* s){
    signal(SIGCHLD,sig_handler);//if child change his status, go to sig handler
    str[strlen(str)-1] = '\0';
    words = countWords(str);
    char **command = (char**)malloc(sizeof(char*) * (words+1));
    command[words] = NULL;
    placeOf(str,command,words);
    x = fork();//forking
    if(x < 0){
        perror("fork fail");
        exit(1);}
    if(x == 0){ //child process
        execvp(command[0],command);
        perror("execvp fail");
        freeM(command,words);
        exit(1);}
    else{ //father process
        freeM(command,words);
        waitReturnPid[*s] = x;//save in pid array the child process to when we 'done'
        (*s)++;//continue in the array location
        }}
//if the son process change his status,we go her
void sig_handler(int sig){
    waitpid(-1,NULL,WNOHANG); //for any child
}
//function to 'nohup' string
void noHup(char str[],pid_t x){
    int w = countWords(str+5);
    if(str[strlen(str)-1] == '&'){ //'nohup' plus '&' in the string
        str[strlen(str)-1] = '\0';
        signal(SIGHUP,SIG_IGN);//ignoring the 'sighup', when we finish the process continue
        signal(SIGCHLD,sig_handler);//when child process change his status, go sig handler function
        char* str1 = (char*) malloc(sizeof(char) * (strlen(str)-5));
        strcpy(str1,str+5);
        str1[strlen(str)-5] = '\0';
        str[0] = '\0';
        strcpy(str,str1);
        str[strlen(str1)] = '\0';
        free(str1);
        w = countWords(str);
        char **command =(char**) malloc(sizeof (char*) * (w+1));
        command[w] = NULL;
        placeOf(str,command,w);
        x = fork();//forking
        if(x < 0){
            perror("fork failed");
            exit(1);}
        if(x == 0) {
            execvp(command[0],command);
            perror("error with execvp");
            exit(1);  }
        else{//the father process doesn't wait, the child process in the background
            freeM(command,w);
        }}

    else{//the 'nohup' string without '&'
     //open file to write in it
    int fd = open("nohup.txt", O_WRONLY|O_APPEND|O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1){
        perror("error open the file");
        exit(1);  }
    char **command =(char**) malloc(sizeof (char*) * (w+1));
    command[w] = NULL;
    placeOf(str+5,command,w);
    x = fork();//forking
    if(x < 0){
        perror("fork failed");
        exit(1);}
    if(x == 0) {//child process
        dup2(fd,STDOUT_FILENO);//changing the output of the child to the text file
        execvp(command[0],command);
        perror("error with execvp");//we go here,the execvp fail
        freeM(command,w);
        exit(1);  }
    else{//father process
        waitpid(x,NULL,0);//father waiting for his specific son process
        close(fd);//closing the path to file
        freeM(command,w);
        }}}
