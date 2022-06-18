# fsop - FileSystem OPerations
---

`fsop` is a command-line utility program for demonstrating various system calls for files in action. `fsop` allows the user to create, read, write and inspect regular, unnamed and named pipes in their target platform's filesystem.

## Execution & Usage

- For execution, ensure that either the executable provided alongside the copy of the program (see [notes](#Notes)) or one built manually for your target architecture (see [building](#Building)) is available.

- ### Usage
  ```bash
  ./fsop.out [-d DIRECTORY] [-q] [-h] {create,read,write,inspect} ...
  ```
  ```bash
  ./fsop.out create [-h] [-p PERMS] [-t {pipe,regular}] [-o] path
  ```
  ```bash
  ./fsop.out read [-h] [-p] [-b OFFSET...] [-B {CUR,END,SET}...] [-c BYTE-COUNT...] [path]
  ```
  ```bash
  ./fsop.out write [--lbuf] [-h] [-A] [-t] [-b OFFSET...] [-B {CUR,END,SET}...] [-p] [path]
  ```
  ```bash
  ./fsop.out inspect [-h] [-l] path...
  ```
- Information about usage, flags and options can also be accessed by executing `./fsop --help` on a terminal.
- More information about actions can be accessed using the `--help` flag alongside the specific action, for example `./fsop.out create --help` to view help on how to use create.

## Examples

- Create a new regular file, named `file.txt`, with permissions `rwx` for all users (simple creation, no data written):
```bash
    ./fsop.out create file.txt --type regular --perms ugo+rwx
```
- Read the complete contents of the file `myfile.txt` (provided it exists):
```bash
    ./fsop.out read myfile.txt
```
- Write input provided on STDIN to offset 40 from the beginning to the file `myfile.txt` (provided file exists):
```bash
    ./fsop.out write myfile.txt --offset 40 --offset-base SET
```
- Inspect all files under the folder `/dev`:
```bash
    ./fsop.out inspect /dev/*
```

## System Call Usage

| Task | System Call(s) Used |
|---|---|
| Create New Files | `creat`, `mknod` |
| Access Existing Files | `open`, `read`, `write`, `lseek` (`lseek64`), `close` |
| Manipulate the Inode | `chdir`, `chmod`, `stat` (`fstat`, `lstat`, `stat64`, ...) |
| Advanced System Calls | `dup`, `pipe`, `unlink` |
| Miscellaneous (non-fs) | `fork`, `getpid`, `signal`, `exit`, `getpwuid`, `getgrgid` |

## Building

- ### Prerequisites
  The following software is required prior compilation of the source:
  - GNU C++ Compiler (g++) version 7 or newer, or any equivalent C++ compiler with support for C++17.
  - GNU Make or equivalent to execute commands from makefiles (optional, recommended). See [notes](#Notes) for a workaround for compilation without Make.
- ### Compilation
  - Prior to compilation, clean up any remaining object and executable files from previously incomplete builds, by running `make clean`.
  - To generate object files and executables, run the default make recipe, using `make`.

## Notes

- The executable provided alongside the project has been compiled on a 64-bit (arch: x86_64) Windows machine running the Windows Subsystem for Linux (WSL) (kernel version: 5.10.102.1-microsoft-standard-WSL2), using the GNU C++ Compiler v9.3.0.
- If GNU make is not installed on the target machine being used for compilation, then either it may be installed via `sudo apt install make` (recommended), or the `make.bash` script supplied alongside the source may be utilized to execute the equivalent Make recipes. To use the script, in the compilation steps replace the usage of `make` with `./make.bash` (e.g.: `make clean` -> `./make.bash clean`).

## About

Created by:
    Kinshuk Vasisht  
    Roll Number: 19  
    M.Sc. Computer Science  
    Department of Computer Science, University of Delhi

## Bibliography

- The Design of the UNIX Operating System, Maurice J. Bach
- Linux Manual Pages for System Calls and Functions: [man7](https://www.man7.org/linux/man-pages/index.html), [linux.die.net](https://linux.die.net/man/)

## Reference

- [GitHub](https://github.com/kinshuk-h/fsop)
