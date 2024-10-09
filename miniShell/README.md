# Minishell

Minishell is a simplified shell program that emulates a Unix-like shell environment. It is built using C and provides a basic command line interface (CLI) where users can interact with the system, execute commands, and manage environment variables. This project also includes implementations of several built-in commands such as `cd`, `echo`, `env`, and more.

## Features

### Command Parsing and Execution
- **Command Line Parsing**: The shell parses user input and supports command execution by interpreting the provided commands and arguments.
- **Tokenization**: Commands are tokenized for proper interpretation, including handling of special characters and operators.
- **Command Expansion**: Supports expanding environment variables and removing quotes from the input commands.
- **Builtin Command Support**: Includes built-in support for common shell commands such as:
  - `cd`: Change the current directory.
  - `echo`: Display a line of text.
  - `env`: Print environment variables.
  - `export`: Set environment variables.
  - `unset`: Unset environment variables.
  - `pwd`: Print the current working directory.
  - `exit`: Exit the shell.
- **Custom Prompt**: Displays the current user, directory, and exit status in the prompt, dynamically updating based on environment variables and user input.

### Signal Handling
- **Signal Management**: The shell can handle various signals like `SIGINT` (Ctrl+C) and `SIGQUIT` to gracefully exit or manage interruptions. The signal handling ensures the shell remains stable during user interactions.
- **Custom Signal Handlers**: Custom handlers manage shell behavior during different states:
  - **Interactive mode**: Default mode during command entry.
  - **Heredoc mode**: Special handling for commands involving here-documents (`<<`).
  - **Child process mode**: Handling signals within child processes.

### Heredoc Support
- **Heredoc Feature**: Implements support for the `<<` operator to allow multiple-line inputs, which is particularly useful for feeding input to commands interactively.

### Environment Variable Management
- **Environment List**: The shell maintains a list of environment variables that can be modified using built-in commands like `export` and `unset`.
- **Dynamic Environment**: Supports dynamically updating the environment, including handling variable expansions within commands.

### Execution of External Commands
- **External Commands**: In addition to built-in commands, the shell supports executing external binaries found in the system's `PATH` variable.

### Error Handling
- **Argument Checking**: The shell checks for valid arguments before executing commands and provides informative error messages for invalid commands or arguments.
- **Memory Management**: Proper memory handling and cleanup to prevent memory leaks and ensure smooth operation.

### Readline Integration
- **History Support**: Maintains command history using the `readline` library, allowing users to scroll through previously executed commands using the up and down arrow keys.
- **Custom Readline Prompt**: The prompt is customized to display user-specific information and can be dynamically updated.

### Compilation and Makefile
- **Makefile Support**: The project comes with a `Makefile` to simplify the compilation process. The Makefile includes the following targets:
  - `all`: Compiles the project and builds the minishell executable.
  - `clean`: Removes object files.
  - `fclean`: Cleans up object files and the compiled binary.
  - `re`: Recompiles the project from scratch.
  - `run`: Runs the shell with memory checking via `valgrind`.

## Compile


 1.Navigate to the project directory.


```bash
cd minishell
```

Compile the project.

```bash
make
```

## Usage


Once the project is compiled, run the shell by executing:

```bash
./minishell
```

Use the shell as you would with any other Unix-like shell by entering commands and pressing `Enter`.

Example:

```bash
$ pwd
/home/user/minishell
$ cd ..
$ echo "Hello, World!"
Hello, World!
```

To exit the shell, use the `exit` command:

```bash
$ exit
```

## Libraries

- **Readline**: This project uses the `readline` library for handling command-line input and history.
- **Libft**: The project relies on `libft`, a custom C library for utility functions such as memory management, string handling, and more.
