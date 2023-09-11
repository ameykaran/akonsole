[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/76mHqLr5)
# AKonsole
## Description

- The prompt is of the form `< <username>@<system>:<directory> [last executed foreground command that took longer than 2 secs] >`

- It has the option to run system commands in background.

- The terminal accepts multi-commands as well as background commands and mixture of both as well.

- Supports paths such as `.`, `..`, `~` and `-` most times.

- Paths can be specified both absolutely or relative to the home dierctory of the terminal.

- IO Redirection is considered for the nearest command

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





# Assumptions
- The max len of any command can only be `ARG_MAX` bytes (131072 in my system)

- The max len of username and system name should be `1024` bytes
