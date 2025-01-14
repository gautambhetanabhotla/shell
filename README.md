[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Qiz9msrr)

# Assumptions

## Spec 1
Prompt format: `username@system-name:directory recent-foreground-process:time $`

## Spec 2
Only tabs and spaces are considered whitespace characters.

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

## Spec 9
- Dynamic aliasing is supported.
- Aliases should be set in the format `alias xx = yy`. Note that spacing matters. Everything must be space-separated. `xx` must be a single word, `yy` can be multiple.
- Typing `alias` will show a list of all aliases made so far.
- `.myshrc` works like a typical run-commands file for any shell, it will run all the commands present in it on startup.

## Spec 10, 11, 12
- Will work as expected for valid commands, but for commands involving `<`, `>` and `>>`, they must be separated by spaces.

## Spec 13
- Running processes are printed in ascending order of their PIDs.

## Spec 14
- None.

## Spec 15
- None.

## Spec 16
- None.

## Spec 17
- None.