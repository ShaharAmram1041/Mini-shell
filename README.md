# Mini-shell
Authored by Shahar Amram


### ==Description==

The program is a  "shell" simulation of linux system.

program contain 3 files:

ex3.c:

The program is a simulation of "shell" of linux system.
The program will get input of sentence/words from the user in infinity loop.
The sentence will be separated by the words.
Memory will be assigned to save the sentence,by every word.
The program will do fork,the "son" process will run the first word that is "shell" command with execvp(), the "father" process wait for his "son" to do his command, and in the end the will free the dynamic memory.
If the given sentence isnt legal command, the execvp() failed,and the dynamic memory will be released (free) by the father,son processes and if the fork will failed. 
we save all the given commands in text file, then there is option to print and show all the history commands by enter "history" command,and activate them with "!"+number command.
illegal commands and "cd" command doesnt show in the file and in the history.
the loop will stop when we will run the command "done".

Forthemore, if the input is '|' one or double, i seperte the string between the '|'. 
the execute will be after forking two children,as i use the pipes between them.
first son will do the execute and write to the pipe,and after, the second son will execute the other word while he reads from the pipe. in the double pipes command i use in the one pipe function,which means i cut the first string till the second '|' and use it in the function,while the second son get the input from the first and continue with execute the other command(in the both cases the father will wate).

with getting string with '&',we run usually like we do in program,just run the command in the background,thats mean after forking to father and son processes,the father does not wait for his son,and when the son changing his status (we want to know) the father will wait for him in specific handler function.
with 'nohup' command we ignoring the input, execute the output to file.
with 'nohup'+'&' command ,we reunning the command in the backgroud plus the command does not finish when our program finish. the command keep running untill her finish.

file.txt:

keeping the history of all commands,if the command illegal,it is not enter.
when i get pipe, i save the all command in the history (the same input i get) ,and in addition seperate each command between the pipes and enter them one by one in the history (to work orgnize with the commands).

output.txt:

the ouput that we get after execute the 'nohup' command. notice that command with 'nohup' and '&' does not appear in the file (only running in the background).


### ==Program DATABASE==

1.cwd = array of char size 512 ,the current position in the computer.

2.str = array of char size 512 , input from the user.

3.str1 = array of char size 512 , using to read from the text file, than copy to str.

4.x = type of pid_t , keeps the result after the fork in the program, navigates us between the processes("father","son",fail in fork).

5.fp = pointer to the file, get access to the file we want to read, write or append.

6.command = dynamic array based on the number of the words, the array keeps the sentence seperate by words.


7.waitReturnPid = array type pid_t size 512,saving all the id's of the son processes that running in the background. when we want finish our program, send sigkill for each of the processes that way i ensure no process will be running in our exit.



### Functions:

1.countWords - this method gets input of char[] and gets the number of words in it.

2.placeOf - this method make an array of dynamic memory from sentence,and keeps each word in different index. 

3.readFromFile - this method gets pointer to file and read it line by line.

4.freeM - function gets pointer to array of dynamic memory and free it, each by each than free the main memory.

5.pipeFun1 - function execute one pipe command.

6.pipeFun2 - function execute two pipes command.

7.removeLastBackSpace - function remove backspace chars from the end of sentence.

8.removeLastBackSpace - function remove backspace chars from the end of sentence.

9.removeInvertedCommas - function remove inverted from the sentence.

10.writeToFile- function get path file and string, write to the path file.

11.changeFromNum - this function work with the history of the commands. get string,and number of pipes in the string and check if there are numbers from history in it,which means the line of the command the user want to do. the function change the numbers in it to there commads location in history, thats way we get the really command that we want, not numbers.

12.ampersand - function execute the '&' command. running it in the background, handle with sigchild (son change his status) , and in 'nohup' case ignoring the 'sighup' signal.

13.sig_handler - function that take care when son changing his status after execute the '&' command.

14.nohup - function execute the 'nohup' command, in case of '&' working with sigchild and sighup for running in the background even when we exit from the program, in the other case execute the output command to 'output.txt' file.

15.fork()- built in function, create two processes 'father'/'son'.

16.pipe()- built in function, create pipe for communicate between processes.

17.dup2()- built in function, change the index of file descripator(input,output,err) to given path. 

18.signal()-catch some signal from process (ignore,handle etc).




### ==Program Files==

ex3.c- the file contain the shell commands with ability to seen the history of the given commands.

file.txt - the file that keeps all the legal commands input from the user in file ex3.c .

ouput.txt - the file will create during the program (if it does not exist),and contain the output 'nohup' commands.




### ==How to compile?==

compile ex3.c: gcc ex3.c -o ex3

run: ./ex3


### ==Input==

sentence - shell command (512 chars).

### ==Output==

ex3.c:

legal command and legal execvp() - the program run the command on the screen exactly like the shell does.
Illegal command or legal command + execvp() failed - the program will print error.
"history" input - print the list from text file.
"nohup" input without '&' - execute command output to 'output.txt' file.
"done" input - print numbers of commands and number of words in the commands enter so far,and finish the program.


