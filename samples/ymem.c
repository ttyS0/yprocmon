#include <stdio.h>
#include <string.h>

int main()
{
    int a[] = { 0, 0, 0, 0 };
    int b[] = { 65, 66, 67, 68 };
    memcpy(a, b, sizeof(b));
    for(int i = 0; i < sizeof(b); i++)
    {
        b[i]++;
    }
    memcpy(b, a, sizeof(a));
    for (int i = 0; i < sizeof(b); i++)
    {
        a[i]++;
    }
    memcpy(b, a, sizeof(a));
    return 0;
}