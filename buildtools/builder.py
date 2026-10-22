from pathlib import Path
from logging import Logger
import shutil
from .utils import run_command


def configure(logger: Logger, build_dir: Path, build_type: str = "Debug") -> None:
    run_command(
        logger,
        [
            "cmake",
            "-B",
            str(build_dir),
            "-G",
            "Ninja",
            f"-DCMAKE_BUILD_TYPE={build_type}",
        ],
    )


def build(logger: Logger, build_dir: Path) -> None:
    run_command(logger, ["cmake", "--build", str(build_dir)])


def clean(logger: Logger, build_dir: Path) -> None:
    if build_dir.exists():
        logger.info(">> Removing build directory")
        shutil.rmtree(build_dir)
    else:
        logger.info(">> Build directory does not exist")


def rebuild(logger: Logger, build_dir: Path, build_type="Debug"):
    clean(logger, build_dir)
    configure(logger, build_dir, build_type)
    build(logger, build_dir)
