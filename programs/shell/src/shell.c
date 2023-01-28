// ? Program that acts as a shell for the OS

#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "hardikhyperionos.h"

int main(int argc, char **argv)
{
    print("HardikHyperionOS v1.0.0\n");

    // Run the shell
    while (1)
    {
        print("> ");
        char buf[1024];
        hardikhyperionos_terminal_readline(buf, sizeof(buf), true);
        print("\n");
        hardikhyperionos_system_run(buf); // Run the command

        print("\n");
    }

    return 0;
}
