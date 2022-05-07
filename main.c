#include <unistd.h>
#include "console.h"

int main ()
{
    Rgb rgb = {12, 34, 56};
    color_print(RGB(23, 45, 67), "start exec !\n");
    print("exec end !\n");
    return 0;
}
