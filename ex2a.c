#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int countWords(char []); // declaration function
void placeOf(char [] , char**, int); // declaration function
void freeM(char**,int); //declaration function
int main() {
    char str[512], cwd[512];
    pid_t x;
    int words, countCommands = 0, sumWords = 0, countLines = 0,p = 0;
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) //get the current position
            printf("%s>", getcwd(cwd, sizeof(cwd)));
        fgets(str, 512, stdin); //input from user
        str[strlen(str) - 1] = '\0';
//error with input command
        if (str[0] == ' ' || str[strlen(str) - 1] == ' ') {
            p = countWords(str);
            if(p<1)
                continue;
            else
                fprintf(stderr, "error with the given command\n");
            continue;}

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
//if the string is the word 'done'
        if ((strcmp(command[0], "done") == 0 && command[1] == NULL && words == 1 && str[0] != ' ' && str[4] != ' ') ||
            strcmp(command[0], "done") == 0 && command[1] == NULL) {
            printf("Num of commands: %d\n", countCommands);
            printf("Total number of words in all commands: %d !\n", sumWords-1);
            freeM(command,words);//free the dynamic memory
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
            wait(NULL); //father is waiting for son process
            freeM(command,words); //free the dynamic memory
            continue;
        }}}

int countWords(char str[]){ //function check how many words in string
    int i ,c = 0,w = 1;
    for(i = 0; str[i] != '\0'; i++){ // loop 'for'- check each word one by one
        if(str[i] != ' ' ) // check if the word is not backspace
            c++;

        if(c > 0  && str[i] == ' '){ // if the word is backspace and its not the first word
            if(str[i-1] != ' ' && str[i+1] != '\0' && str[i+1] != ' '){ // the current word is backspace and before+after there is word different from backspace
                w++;
                continue;}

            if(str[i-1] == ' ' && str[i+1] != '\0' && str[i+1] != ' '){ // before the current word there is space and after there isnt backspace
                w++;
                continue;}}}
    if(c == 0) // if there are no words
        w = 0;
    return w;}
//make array of the words from the given sentence
void placeOf(char str[], char **command,int words) {
    int i, c = 0, j = 0;
    for (i = 0; str[i] != '\0'; i++) { //check each char
        if (str[i] != ' ' && str[i + 1] != '\0')
            c++;

        if (str[i] == ' ' && words > 1 && i != 0) { // if the word is backspace and its not the first word
            if (str[i - 1] != ' ' && str[i + 1] != '\0' && str[i + 1] != ' ') {
                command[j] = (char *) malloc(sizeof(char));//make dynamic memory
                if(command[j] == NULL){
                    perror("malloc failed");
                    exit(1);}
                command[j][0]='\0';
                strncpy(command[j], str + (i - c), c);//copy the string from thw wanted index
                command[j][c] = '\0';
                j++;
                words--;
                c = 0;
                continue;}
            if (i - 1 >= 0 && str[i - 1] != ' ' && str[i + 1] != '\0' && str[i + 1] == ' ') {
                command[j] = (char *) malloc(sizeof(char));//make dynamic memory
                if(command[j] == NULL){
                    perror("malloc failed");
                    exit(1);}
                command[j][0]='\0';
                strncpy(command[j], str + (i - c), c);//copy the string from thw wanted index
                command[j][c] = '\0';
                j++;
                words--;
                c = 0;
                continue;}
            else
                continue;}
        if (words <= 1 && str[i] != ' ') {//  words ==1
            command[j] = (char *) malloc(sizeof(char));//make dynamic memory
            if(command[j] == NULL){
                perror("malloc failed");
                exit(1);}
            command[j][0]='\0';
            strncpy(command[j], str + i, strlen(str) - i);//copy the string from thw wanted index
            command[j][strlen(str) - i] = '\0';
            break;}}}
//function free the dynamic memory
void freeM(char **command,int w){
    int i;
    for(i = 0; i<=w ; i++){
        free(command[i]);//free each of the dynamic array
        command[i] = NULL;}
    free(command); //free the main memory
    command = NULL;}
