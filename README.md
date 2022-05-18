# fsop - FileSystem OPerations
---

`fsop` is a command-line utility program for demonstrating various system calls for files in action. `fsop` allows the user to create, read, write and inspect regular, unnamed and named pipes in their target platform's filesystem.

## Execution & Usage

- For execution, ensure that either the executable provided alongside the copy of the program (see notes) or one built manually for your target architecture (see [building](#Building)) is available.

- ### Usage
  ```bash
  ./fsop 
  ```
- Information about usage, flags and options can also be accessed by executing ``` ./fsop --help ``` on a terminal.

## Building

- ### Prerequisites
  The following software is required prior compilation of the source:
  - GNU C++ Compiler (g++), version 4.9 or newer, or any equivalent C++ compiler with support for C++11.
  - GNU Make or equivalent to execute commands from makefiles (optional, recommended). See notes for a workaround for compilation without Make.
- ### Compilation
  - Prior to compilation, clean up any remaining object and executable files from previously incomplete builds, by running ```make clean```.
  - To generate object files and executables, run the default make recipe, using ```make```.

## Notes

- The executable provided alongside the project has been compiled on a 64-bit (arch: x86_64) Windows machine running the Windows Subsystem for Linux (WSL) (kernel version: ), using the GNU C++ Compiler v.
- If GNU make is not installed on the target machine being used for compilation, then the `make.bash` script supplied alongside the source may be utilized to execute the equivalent Make recipes. To use the script, in the compilation steps replace the usage of `make` with `./make.bash` (e.g.: `make clean` -> `./make.bash clean`).

## About

Created by:
    Kinshuk Vasisht  
    Roll Number: 19  
    M.Sc. Computer Science  
    Department of Computer Science, University of Delhi

## Bibliography

- The Design of the UNIX Operating System, Maurice J. Bach
