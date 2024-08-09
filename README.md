# Terminal-Text-Editor-C
A simple text editor for the terminal, coded using the C programming language.

## Commands
- Ctrl+Q to quit
- Ctrl+S to save file
- Ctrl+F to search for word/character

## Features

- Open/Create files
- Save/Discard changes
- Syntax Highlights (C and Makefile)
- Line Numbering

## How to Build

### Prerequisites
Since this program was built using raw C, it only requires:
- GCC (GNU Compiler Collection)
- Make

### Compile
```
terminal> cd <project_location>
terminal> make
```

### Run
It is possible to open an empty editor, or an existing text-file.
```
terminal> ./TextEditor 
```
```
terminal> ./TextEditor <file_name>
```

## TO-DO
- Automatic indentation
- Makefile highlights
- Fix 'tab' characer deletion bug
- Support for multiple files
