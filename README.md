            .??.    ~J?J?.      
           :G@@^  :5@@@B~       
          :B@@&E\.7#@@&?          
         ^#@@; ^P@@@5:             
        ^#@@:   @@@#^             AKonsole 
       ~&@@@5??^5@@@@Y:             the new age terminal!
      !&@@@@^~*^*~G@@@&?.
     7@@@G!        7#@@@#!.     
    :Y55Y3          .J5555~     


## Table of Contents
- [Introduction](#introduction)
- [Installation](#installation)
- [Features](#description)
- [Commands](#commands)
- [Assumptions](#assumptions)


## Introduction
AKonsole is a terminal emulator written in C that supports most of the basic functionalities of a terminal. It is a part of the assignment in Operating Systems and Networks course at IIIT Hyderabad.


## Installation
1. Clone the repository
2. Run `make` in the root directory
3. Run `./akonsole` to start the terminal


## Description

- The prompt is of the form `< <username>@<system>:<directory> [last executed foreground command that took longer than 2 secs] >`

- It has the option to run system commands in background.

- The terminal accepts multi-commands as well as background commands and mixture of both as well.

- Supports paths such as `.`, `..`, `~` and `-` most times.

- Paths can be specified both absolutely or relative to the home dierctory of the terminal.

- Prints the output of the background process only on the next input

- IO Redirection is considered for the nearest command

- Multiple commands can be piped. 


## Commands

### warp

Syntax `warp [path [path1 [path2 ...]]]`

Changes the current working directory to `path`.

* If `path` is not mentioned, changes to the home directory.

* Multiple paths can be passed which get executed sequentially.


### peek

Syntax `peek <flags> [path]`

Lists down the files and directories present in the `path`

- Flags
    - `-a` include hidden files and directories
    - `-l` print more detailed information

- Colour coding
    - blue for directory
    - cyan for symbolic link
    - green for executable
    - white for regular file

1. Total in the `peek -l` command doesn't actually print the disk usage. It prints the total number of entries in the directory.

2. `peek -` is an invalid command


### pastevents

Family of commands that store the recently executed commands. 
    

Variations 
- `pastevents` - prints out the history with the most recent command being at the bottom.
  
- `pastevents purge` - clears the history

- `pastevents execute <ind>` - executes the most recent `ind`<sup>th</sup> command



1. Any command from the `pastevents` family is not saved.

2. Commands are not repeated consecutively.

3. Running `pastevents execute` command replaces it with the corresponding previous command and runs the newly generated command.

4. If an invalid number is passed in the `execute` subcommand, or if the `ind`<sup>th</sup> command is not saved, `execute` is replaced with a blank string.

5. Saves the command irrespective of it being invalid or syntactically incorrect.



### proclore
Syntax: `proclore [pid]`

Prints the info about a particular process with `pid`.

* Prints the process ID, process status, process group, virtual memory size and the path of the executable.

* Prints the info about the terminal if `pid` is not given.



1. A process is a foreground process if its process group id is equal to the terminal's pid


### seek
Syntax: `seek <flags> name [path]`

Searches in the `path` specified for items having `name` as a prefix.


* Flags
    
    * `-d` Search only for directories
    * `-f` Search only for files
    * `-l` Search only for symbolic links
    * `-h` Search for hidden files as well (hidden files are not searched for by default)
    * `-e` Effective only when the search results in a single item. Performs these actions when the item is a 
        
        * directory - changes directory to it 
        * file - prints it out 
        * symbolic link - follows it and performs the same actions as the above two.


1. Symbolic links are not followed while searching

2. Only one of the `d`, `f` and`l` flags can be passed at a time. However, when not mentioned, the default flag is `dfl`.

3. `h` and `e` have to be specified explicitly.



### I/O Redirection
Syntax: `<command> [< input] [> output] [>> append] [<<< text]`

Redirects the input and/or output to the file speciified.

* Works with input file or even input text (given by `<<<`).

* Output can be redirected to either a new file or can be appended to an existing file.

* Only one of the two input options must be given at a time. Similarly, only one output type has to be provided.



### Activities
Lists down of all the processes spawned by AKconsole


### Ping
Syntax: `ping <pid> <signal>`
Sends signal `signal` to the process with ID `pid`


### Ctrl - C signal
Kills all processes by sending a `SIGINT` signal to them


### Ctrl - D signal
Exits the terminal after killing all processes.


### Ctrl - Z signal
Pushes any running foreground process to the background and changes its state from “Running” to “Stopped” 


### fg
Syntax: `fg <pid>`

Brings the running or stopped background process with corresponding pid to foreground, handing it the control of terminal. 


### bg
Syntax: `bg <pid>`

Changes the state of a stopped background process to running (in the background). 



### Neonate
Syntax: `neonate [time]`

Prints the PID of the most recently created process on the system every `time` seconds until the key `x` is pressed.

* Default time is 0 seconds if not specified.

* Input redirection cannot be done.



### iMan
Syntax: `iman <command>`

Fetches the manpage of the specified `command` from `http://man.he.net/` and prints it.

* Hyperlinks are working. i.e. they point to their respective addresses.

* Text is bold wherever necessary.

* Hyperlinked words are underlined to differentiate them.



# Assumptions
- The max len of any command can only be `ARG_MAX` bytes (131072 in my system)

- The max len of username and system name should be `1024` bytes

- Piping and ampersand (background process) should not be given together.

- For combination of IO redirection and pipes, IO redirection is given preferance over pipe to the nearest command.

- The pipeline ignores an invalid pipelet and executes as if it wasn't there.

---
Some errors I've found
- `cat <<< "text" | wc` doesn't work whereas `cat <<< 'text' > a | cat a | wc` works perfectly.

---