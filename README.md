# RISC-V Emulator

An educational RISC-V emulator written in C++. It models a small CPU and byte-addressable memory, and demonstrates a basic fetch-decode-execute cycle. The project is a work in progress: it is not a complete or fully validated implementation of the RISC-V specification.

## Features

- 32 32-bit CPU registers, a program counter, and a reset operation.
- Byte-addressable memory with byte and 32-bit read/write operations and bounds checking.
- Instruction fetch, decoding, and execution for a growing subset of RISC-V instructions, including arithmetic and logic, branches, jumps, loads, and stores.
- A hard-coded demonstration program in `src/main.cpp`.
- GoogleTest-based unit tests for basic CPU instruction execution and memory access/bounds behavior.

Instruction coverage and behavior are still evolving. Some instruction paths are incomplete; this emulator should not be used as a reference implementation or expected to run arbitrary RISC-V binaries.

## Requirements

- A C++17-compatible compiler.
- CMake 3.20 or later.
- Python 3.9 or later to use the `riscv-emu.py` helper.
- Ninja to use the Python helper, which configures CMake with the Ninja generator.
- An internet connection on the first CMake configure, to download the pinned GoogleTest dependency through CMake `FetchContent`.

Make sure the required tools are available from your terminal. If you configure CMake directly, you may use a generator other than Ninja.

## Build and run

From the repository root, use the Python helper:

```powershell
python riscv-emu.py debug
python riscv-emu.py run
```

`debug` configures and builds the Debug version; `run` starts the previously built demonstration executable. The program's output includes memory values and information about executed instructions.

To build and run the Release version:

```powershell
python riscv-emu.py release
python riscv-emu.py run
```

The helper supports these commands:

| Command | Action |
| --- | --- |
| `debug` | Configure and build a Debug version. |
| `release` | Configure and build a Release version. |
| `rebuild` | Remove the `build/` directory, then configure and build Debug. |
| `run` | Run the previously built emulator executable. |
| `run-test` | Run the previously built test executable. |
| `clean` | Remove the `build/` directory. |

Both executables are placed in `bin/`. The `clean` command removes only `build/`; it does not remove generated executables from `bin/`.

## Testing

Build the project and run the test executable with the helper:

```powershell
python riscv-emu.py debug
python riscv-emu.py run-test
```

Or use CMake and CTest directly:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

CMake discovers the GoogleTest test cases during configuration, so CTest can run the discovered tests individually. The current tests check a CPU `ADDI` instruction and memory byte reads/writes, including out-of-range access.

## Project structure

```text
CMakeLists.txt          CMake targets, GoogleTest setup, and test discovery
src/
  cpu.h, cpu.cpp        CPU state, instruction decoding, and execution
  memory.h, memory.cpp  Byte-addressable memory implementation
  main.cpp              Hard-coded emulator demonstration
tests/
  test_cpu.cc           CPU tests
  test_memory.cc        Memory tests
buildtools/
  builder.py            CMake configure/build/clean helpers
  runner.py             Executable runner
  utils.py              Shared command and logging helpers
riscv-emu.py            Command-line interface for the helper
```

## Contributing

Contributions are welcome. Please create a branch, describe the change clearly, and run the relevant build and tests before opening a pull request.
