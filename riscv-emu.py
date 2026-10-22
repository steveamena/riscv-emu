from logging import Logger
from pathlib import Path
import argparse
import traceback
import sys

from buildtools import configure, build, configure_logger, run, rebuild, clean


def arguments_manager(logger: Logger):
    parser = argparse.ArgumentParser(
        description="RISC-V build and runner tool"
    )

    parser.add_argument(
        "command",
        choices=["debug", "release", "rebuild", "run", "clean"],
        help="Command to execute",
    )

    args = parser.parse_args()

    logger.info(args.command)
    return args


def main():
    root_dir = Path(__file__).resolve().parent
    build_dir = root_dir / "build"
    binary_dir = root_dir / "bin"

    executable_name = "riscv-emu"

    # Windows executable extension
    if sys.platform.startswith("win"):
        executable_name += ".exe"

    executable_path = binary_dir / executable_name
    logger = configure_logger("buildtools")
    args = arguments_manager(logger)
    commands = {
        "debug": lambda: (
            configure(logger, build_dir, "Debug"),
            build(logger, build_dir),
        ),
        "release": lambda: (
            configure(logger, build_dir, "Release"),
            build(logger, build_dir),
        ),
        "rebuild": lambda: rebuild(logger, build_dir, "Debug"),
        "run": lambda: run(logger, executable_path),
        "clean": lambda: clean(logger, build_dir),
    }
    try:
        commands[args.command]()
    except Exception as e:
        print(f"Exception {e} generated. Details: {traceback.format_exc()}")


if __name__ == "__main__":
    main()
