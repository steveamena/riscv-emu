from pathlib import Path
from logging import Logger
from .utils import run_command


def run(logger: Logger, executable_path: Path) -> None:
    if not executable_path.exists():
        logger.error("Executable not found. Build the project first.")
        return
    run_command(logger, [str(executable_path)])
