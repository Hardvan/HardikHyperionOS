#include "hardikhyperionos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char **argv)
{
    // Display the argument 50 times
    for (int a = 0; a < 50; a++)
    {
        print(argv[0]);
        for (int i = 0; i < 100000; i++) // Delay
        {
        }
    }

    return 0;
}
