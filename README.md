# Command descriptions

### prompt

<Username@SystemName:~>

### peek
- Colour coding
    - blue for directory
    - cyan for symbolic link
    - green for executable
    - white for regular file

1. Total in the `peek -l` command doesn't actually print the disk usage. It prints the total number of entries in the directory.

- [x] Implement the normal and detailed versions
- [ ] Handle errors
- [x] Add flags



seek
symlinks supported
hidden files also searched



seek doesnot follow the symlinks


# Assumptions
- The max len of any command can only be `ARG_MAX` bytes (131072 in my system)

- The max len of username and system name should be `1024` bytes



## todo
# warp
strip space, forward slash
multiple paths

# pastevents
purge
execute

# peek 
error handling





realpath

ctime