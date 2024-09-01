[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Qiz9msrr)
# mini-project-1-template

## Assumptions
- `exit` command is logged.
- A process is considered a background process if its controlling terminal is 0. Although this is not always the case, we will assume so for this mini project.
- For some processes, permission for obtaining paths to their executables is denied.
- Invalid flags in `reveal` are ignored.
- Symbolic links are also represented in `ls -l`
- File paths won't be longer than 4096 characters including null
- Command strings wont be longer than 4096 characters

## Spec 1
Prompt format: `username@system-name:directory recent-foreground-process:time $`

## Spec 3
- When given multiple arguments to hop, it sequentially moves to those directories until an erroneous path is given.

## Spec 4
- A file is considered hidden if and only if its name starts with `.`
- `reveal -l` will also print the destination of symbolic links, like bash's `ls -l`.
- This code works for an arbitrary number of directory entries (as it uses dynamic memory allocation).
- Using the `-` argument as a directory will raise an `Invalid path` error if no previous working directory exists.

## Spec 5
- Erroneous commands are also logged.
- The command corresponding to the index in `log execute <index>` also gets logged while running `log exeute`.
- `log execute n` executes the `n`th most recent command.
- `log` displays logged commands, newest at the bottom.
- The `exit` command is logged.
  
## Spec 6
- PIDs range from 1 to 9999999.
- On running an erroneous system command in the background, it will first show the error then say that the erroneous process exited abnormally, as it should.

## Spec 7
- A process is considered a background process if its controlling terminal is 0. Although this is not always the case, it is a reasonable assumption.
- To obtain all the information about some processes, like executable paths, it is required to run the shell in `sudo`, otherwise it will raise a `Permission denied` error.
- The `Virtual Memory` field shows the number of **bytes** of memory that the process is using.

## Spec 8
- Executables won't be searched for, with the `-e` flag.