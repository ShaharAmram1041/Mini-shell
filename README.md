# Mini-shell
Authored by Shahar Amram


### ==Description==

The program is a  "shell" simulation of linux system.
program contain 2 files:

ex2a.c:

The program will get input of sentence/words from the user in infinity loop.
The sentence will be separated by the words.
Memory will be assigned to save the sentence,by every word.
The program will do fork,the "son" process will run the first word that is "shell" command with execvp(), the "father" process wait for his "son" to do his command, and in the end the will free the dynamic memory.
If the given sentence isnt legal command, the execvp() failed,and the dynamic memory will be released (free) by the father,son processes and if the fork will failed.


ex2b.c:

That program run exactly like ex2a.c but with extra: ability to read the given commands.
In the file ex2b.c we save all the given commands in text file, then there is option to print and show all the history commands by enter "history" command,and activate them with "!"+number command.
illegal commands and "cd" command doesnt show in the file and in the history.
the loop will stop when we will run the command "done". </sub>


### ==Program DATABASE==

1.cwd = array of char size 512 ,the current position in the computer.

2.str = array of char size 512 , input from the user.

3.str1 = array of char size 512 , using to read from the text file, than copy to str (only in ex2b.c file).

4.x = type of pid_t , keeps the result after the fork in the program, navigates us between the processes("father","son",fail in fork).

5.fp = pointer to the file, get access to the file we want to read, write or append.(only in ex2b.c file).

6.command = dynamic array based on the number of the words, the array keeps the sentence seperate by words.

<!-- send the array to the execvp() - the first word and the array. -->


### Functions:

1.countWords - this method gets input of char[] and gets the number of words in it.

2.placeOf - this method make an array of dynamic memory from sentence,and keeps each word in different index.

3.readFromFile (only in file ex2b.c) - this method gets pointer to file and read it line by line.

4.freeM - function gets pointer to array of dynamic memory and free it, each by each than free the main memory.

5.fork - built in function forking/split the program to father and son processes, exactly from the line after the fork.



### ==Program Files==

ex2a.c- the file contain the shell commands without seen the history of the commands.

ex2b.c- the file contain the shell commands with ability to seen the history of the given commands.

file.txt - the file that keeps all the legal commands input from the user in file ex2b.c.


### ==How to compile?==

compile ex2a.c: gcc ex2a.c -o ex2a

run: ./ex2a

compile ex2b.c: gcc ex2b.c -o ex2b

run: ./ex2b

### ==Input==

sentence - shell command (512 chars).

### ==Output==

ex2a.c:

legal command and legal execvp() - the program run the command on the screen exactly like the shell does.
Illegal command or legal command + execvp() failed - the program will print error.
"done" input - print numbers of commands and number of words in the commands enter so far.

ex2b.c:

legal command and legal execvp() - the program run the command on the screen exactly like the shell does.
Illegal command or legal command + execvp() failed - the program will print error.
"history" input - print the list from text file.
"done" input - print numbers of commands and number of words in the commands enter so far.


