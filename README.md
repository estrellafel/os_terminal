# Shell Project

## Author:

Felix Estrella


## Date:

07/06/22


## Description:

This is a program that is a shell, like bash for example. It can take in a bunch of differnet commands such as ls, date, and many more. It can run commands in the foreground and in the background. Also, this program has a few builtin commands that are exit, fg, history, jobs, and wait. Plus, this program can do file redirection. Lastly, the program supports interactive mode which takes user input and batch mode which takes in one or more files.


## How to build the software

To build the software is by calling on the make file. The command that should be called make.
```console
    make
```


## How to use the software

How to run the code, is to call the executable on the command line with nothing extra for interactive mode and with the file name(s) after for batch mode.

Interactive Mode Example:  
```console
    ./mysh
```
Batch Mode Examples: 
```console
    ./mysh test.txt
    ./mysh test.txt file.txt
```
