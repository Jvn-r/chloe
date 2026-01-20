# chloe

chloe is my minimal Windows command-line shell written in C, built to learn more about process execution, and command parsing for builtin commands and external commands etc. in C

Active development happening rn.

## Motivation
This project was created to gain a deeper, hands-on understanding of:
- Windows process creation and termination lifecycle
- Command parsing and tokenization 
- Pointer and memory semantics in C
- How real shells interact with the operating system in WIndows

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

## Planned Features
- Builtin commands
  - exit
  - cd
  - pwd
  - echo
- Command Chaining
- Better error management
- Pipes

## Docs
- Heavily processed implementation notes in `docs` 

## BUGS
- FIXED : `echo hello;pwd` will not work, `echo hello ;pwd` or anything with a space behind the operator works, will fix it in the near future