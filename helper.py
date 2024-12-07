import argparse
import sys
import os
import subprocess
import shutil
from typing import NoReturn

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))
PROJECT_NAME = "general_ll1"
PROJECT_BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
DEFAULT_PROJECT_CONFIG = "Dev"
MAIN_EXECUTABLE = PROJECT_NAME + ".exe"

def get_project_binary_dir(conifg: str) -> str:
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


def build_custom(config: str, target: str):
    assert_installed("cmake") 

    assert target in ["all", "main", "tests"]

    actual_target = {
        "all": "ALL_BUILD",
        "main": PROJECT_NAME,
        "tests": "tests"
    }[target]

    run_command(f"cmake -S {PROJECT_ROOT} -B {PROJECT_BUILD_DIR}")
    run_command(f"cmake --build {PROJECT_BUILD_DIR} -j 4 --config {config} --target {actual_target}")

def command_build(parsed_args) -> None:
    build_custom(parsed_args.config, parsed_args.target)

def command_run(parsed_args, rest_args):
    if parsed_args.build:
        build_custom(parsed_args.config, "main")

    exe_full_path = os.path.join(get_project_binary_dir(parsed_args.config), MAIN_EXECUTABLE) 
    exe_args = " ".join(rest_args)

    run_command(f"\"{exe_full_path}\" {exe_args}", end_on_fail=False)

def command_test(parsed_args, rest_args):
    assert_installed("ctest")

    if parsed_args.build:
        build_custom(parsed_args.config, "tests")

    test_args = " ".join(rest_args)
    run_command(f"ctest -C {parsed_args.config} {test_args}", cwd=PROJECT_BUILD_DIR)

def command_debug(parsed_args, rest_args):
    assert_installed("WinDbgX") 
    
    if parsed_args.build:
        build_custom("Debug", "main")

    exe_full_path = os.path.join(get_project_binary_dir("Debug"), MAIN_EXECUTABLE) 
    exe_args = " ".join(rest_args)

    print(f"\nRunning WinDbgX {exe_full_path} {exe_args}\n")
    subprocess.Popen(["WinDbgX", exe_full_path, *rest_args])

def main() -> None:
    parser = argparse.ArgumentParser(
                        prog="Helper",
                        description="Automates common tasks")
    subparsers = parser.add_subparsers(dest="command", help='Subcommands')
    
    build_parser = subparsers.add_parser("build", help="Build source code")
    build_parser.add_argument("-C", "--config", dest="config", default=DEFAULT_PROJECT_CONFIG)
    build_parser.add_argument("-T", "--target", dest="target", choices=["all", "main", "tests"], default="main")

    run_parser = subparsers.add_parser("run", help="Runs compiled executable")
    run_parser.add_argument("-C", "--config", dest="config", default=DEFAULT_PROJECT_CONFIG)
    run_parser.add_argument("-B", "--build", dest="build", action=argparse.BooleanOptionalAction)

    test_parser = subparsers.add_parser("test", help="Runs tests")
    test_parser.add_argument("-C", "--config", dest="config", default=DEFAULT_PROJECT_CONFIG)
    test_parser.add_argument("-B", "--build", dest="build", action=argparse.BooleanOptionalAction)

    debug_parser = subparsers.add_parser("debug", help="Launches debugger")
    debug_parser.add_argument("-B", "--build", dest="build", action=argparse.BooleanOptionalAction)

    args, rest_args = get_rest_args(sys.argv)
    parsed_args = parser.parse_args(args);

    match parsed_args.command:
        case "build":
            command_build(parsed_args)

        case "run":
            command_run(parsed_args, rest_args)

        case "test":
            command_test(parsed_args, rest_args)
    
        case "debug":
            command_debug(parsed_args, rest_args)


if __name__ == "__main__":
    main()
