# chloe

chloe is my minimal Windows command-line shell written in C, built to learn more about process execution, and command parsing for builtin commands and external commands and more in C.

not just a command-line shell anymore, chloe is a shell engine that runs on a WIN32 UI.

Active development happening rn.

## Motivation
This project was created to gain a deeper, hands-on understanding of:
- Windows process creation and termination lifecycle
- Command parsing and tokenization 
- Pointer and memory semantics in C
- How real shells interact with the operating system in WIndows
- Win32 UI in C

## Current Features
- Taking user input using `fgets` and handling newline char left behind by `fgets`
- Tokenization of input into `argv` argument array
- Process creation using `CreateProcessA`
- Differenciation from user input variable and tokenized variable
- `exit` will exit
- in built funcs will be handled seperately
- `echo` command function added
- `pwd` will print working directory
- `cd` will change directory now
- new better revamped upgraded have i said new Tokenizer - lexical char by char tok instead of `strtok`
- a command builder and executionar handle command building and executioning instead of argv and raw input line
- "improvements" to understanding of tokenizer function
- rebuilding command input in `create_proc` fn
- exit codes in `create_proc`
- operators `;`,`&&`,`||` all work as intended
- standardized return codes `0 = success and non zero = failure` 
- more in built fns `help`, `hello`, `ver` and `calc`
- made a Win32 window UI 
- auto clearing input(on Enter press) and scrollable output(with emoticons)
- chloe to library, removed main and condensed chloe into 1 callable func
- chloe and UI to UNICODE `W` no ANSI `A`
- testing.c to test the library 
- removed every `stdin` and `stdout` to terminal from `chloe.c`
- chloe talks only to ui.c or any others who call upon her using the `call_chloe` func
- `scribble` func to write into output buffer
- fully connected UI and Library

## Planned Features
- Builtin commands
  - exit
  - cd
  - pwd
  - echo
- Command Chaining
- Better error management
- Pipes
- UI using da Win32 API
  - `DONE` - finishing UI - input - chloe - proc - output - UI pipeling, half is done for now jst need output from chloe into UI - `DONE`

## Docs
- Heavily processed implementation notes in `docs` 

## BUGS
- FIXED : `echo hello;pwd` will not work, `echo hello ;pwd` or anything with a space behind the operator works, will fix it in the near future
- `calc` in built function not updated, will do later

## Releases
- v1.0 : primitive shell, basics only, but it all works

## Notes to Run
- `gcc -municode Chloe.c ui.c -o ui ;./ui` - chloe + ui RAHHHHH
- `gcc Chloe.c testing.c -o test ;./test` change command in test