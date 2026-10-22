from pathlib import Path
import logging
from logging import Logger
import subprocess


def run_command(logger: Logger, command: list[str]) -> None:
    logger.info(">> Running: %s", " ".join(command))
    subprocess.run(command, check=True)


def configure_logger(logger_name: str) -> Logger:
    logging.basicConfig(level=logging.INFO, format="[%(levelname)s] %(message)s")
    return logging.getLogger(logger_name)
