cmake_minimum_required(VERSION 3.27)
project(docker_cli_tool C)

set(CMAKE_C_STANDARD 11)

add_executable(docker_cli_tool main.c
        lib/globals.h
        lib/commands/help/help_command.c
        lib/commands/help/help_command.h
        lib/commands/generate/generate_command.c
        lib/commands/generate/generate_command.h
        lib/files/file_manager.c
        lib/files/file_manager.h
)
