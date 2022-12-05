/* MemoryManagement von Philipp Duspiwa und Benjamin Zastrow
*  Version 1.0 ("Kangaroo")
*/


#include <stdio.h>
#include "MM.h"

int main()
{
    mminit(310, 4);
    char* test1 = mmmalloc(50, "Philipp");
    char* test2 = mmmalloc(40, "kann");
    char* test3 = mmmalloc(50, "SWE");
    char* test4 = mmmalloc(50, "ganz");
    char* test5 = mmmalloc(60, "gut");
    char* test6 = mmmalloc(50, ", oder?");
    mmdump();
    mmrename(test1, "Harry");
    mmdump();
    mmfree(test4);
    mmfree(test5);
    mmdump();
    mmterm();
    return 0;
}
