# RISC-V Emulator

An educational RISC-V emulator written in C++. The project implements the initial components of a CPU—register file, memory, and the fetch-decode-execute cycle—along with a Python tool to configure, build, and run the program.

> This project is a work in progress. The current implementation is a prototype and does not yet provide full compatibility with the RISC-V specification.

## Current features

- CPU with 32 32-bit registers and a program counter.
- Byte-addressable memory with operations for reading and writing bytes and 32-bit words.
- An execution cycle that fetches an instruction from memory, decodes it, and executes it.
- Initial support for register arithmetic and logic instructions (`ADD`, `SUB`, `XOR`, `OR`, `AND`), immediate addition (`ADDI`), and jumps (`JAL`, `JALR`).
- Initial code for load and store instructions.
- A demo program in `src/main.cpp` that loads and executes a small set of test instructions.

Although the decoder recognizes additional opcodes, execution is not yet implemented for all of them.

## Requirements

- C++14 or later.
- CMake 3.20 or later.
- Ninja.
- Python 3.9 or later.

Make sure `cmake`, `ninja`, and `python` are available from your terminal.

## Build and run

From the repository root, run the following commands in PowerShell or another terminal:

```powershell
python riscv-emu.py debug
python riscv-emu.py run
```

The first command configures and builds the Debug version. The executable is placed in `bin/`. The second command runs that executable.

The following commands are also available:

| Command | Action |
| --- | --- |
| `debug` | Configures and builds the Debug version. |
| `release` | Configures and builds the Release version. |
| `rebuild` | Removes the build directory and rebuilds in Debug mode. |
| `run` | Runs the previously built executable. |
| `clean` | Removes the `build/` directory. |

For example, to build and run the Release version:

```powershell
python riscv-emu.py release
python riscv-emu.py run
```

## Project structure

```text
src/
  cpu.cpp       CPU implementation and instruction execution
  cpu.h         CPU interface and instruction structure
  main.cpp      Demo program
  memory.cpp    Memory implementation
  memory.h      Memory interface
buildtools/     Python helpers for configuring, building, and running
riscv-emu.py    Command-line interface for the build tool
CMakeLists.txt  CMake project configuration
```

## Contributing

Contributions are welcome. To propose a change, create a branch, make and validate your changes, and open a pull request with a clear description.
