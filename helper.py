import argparse
import sys
import os
import subprocess
import shutil
from typing import NoReturn

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))
PROJECT_NAME = "general_ll1"
PROJECT_BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
DEFAULT_PROJECT_CONFIG = "Debug"
MAIN_EXECUTABLE = PROJECT_NAME + ".exe"

def get_project_binary_dir(conifg = DEFAULT_PROJECT_CONFIG) -> str:
    return os.path.join(PROJECT_BUILD_DIR, "bin", conifg)

def get_rest_args(args: list[str]) -> tuple[list[str], list[str]]:
    try:
        index = next(i for i, arg in enumerate(args) if arg == "--")
        return args[1:index], args[index + 1:]
    except:
        return args[1:], []

def assert_installed(name: str) -> None|NoReturn:
    print(f"Checking for {name} in your $PATH")

    if shutil.which(name) is None:
        print(f"{name} is not isntalled in your system")
        exit(1)

def run_command(command: str, end_on_fail=True, cwd=None) -> None|NoReturn:
    print(f"\nRunning {command}\n")
    status = subprocess.call(command, cwd=cwd)
    if end_on_fail and status != 0:
        print(f"Failed with status code: {status}")
        exit(1)

def command_build(_parsed_args) -> None:
    assert_installed("cmake") 

    run_command(f"cmake -S {PROJECT_ROOT} -B {PROJECT_BUILD_DIR}")
    run_command(f"cmake --build {PROJECT_BUILD_DIR} -j 4")

def command_run(_parsed_args, rest_args):
    exe_full_path = os.path.join(get_project_binary_dir(), MAIN_EXECUTABLE) 
    exe_args = " ".join(rest_args)

    run_command(f"\"{exe_full_path}\" {exe_args}", end_on_fail=False)

def command_test(_parsed_args, rest_args):
    assert_installed("ctest")

    test_args = " ".join(rest_args)
    run_command(f"ctest -C {DEFAULT_PROJECT_CONFIG} {test_args}", cwd=PROJECT_BUILD_DIR)

def main() -> None:
    parser = argparse.ArgumentParser(
                        prog="Helper",
                        description="Automates common tasks")
    subparsers = parser.add_subparsers(dest="command", help='Subcommands')
    
    build_parser = subparsers.add_parser("build", help="Build source code")
    run_parser = subparsers.add_parser("run", help="Runs compiled executable")
    test_parser = subparsers.add_parser("test", help="Runs tests")


    args, rest_args = get_rest_args(sys.argv)
    parsed_args = parser.parse_args(args);

    match parsed_args.command:
        case "build":
            command_build(parsed_args)

        case "run":
            command_run(parsed_args, rest_args)

        case "test":
            command_test(parsed_args, rest_args)
    


if __name__ == "__main__":
    main()