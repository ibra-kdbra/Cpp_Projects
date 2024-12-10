# Mojan Text Editor

A simple, minimal & performant text editor written in C with only C's Standard Library.

![Screenshot](./img/captured.png)

_**Note**: This project is undergoing refactoring at the moment._

## Syntax highlighting supported for

- C/C++
- JavaScript
- Python
- SQL

## Installation

- Make sure that `gcc` & `make` are installed.
- Make _install.sh_ file executable.
  ```
  chmod +x install.sh
  ```
- Execute the installation script.
  ```
  ./install.sh
  ```

## Build

```
make mojan
```

## Usage

```
mojan [FILE]
```

## Planned Features

- Undo & redo
- Text replace
- Colored bracket pair
- Syntax highlightng for other languages like Cpp, Java, TS, Julia, C#, many more...