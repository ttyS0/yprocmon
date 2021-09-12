#include <windows.h>

int main()
{
    int a[] = { 1, 2, 3, 0 };
    int b[] = { 4, 5, 6, 0 };
    MessageBox(NULL, L"666", L"777", 0);
    memcpy(a, b, sizeof(a));
    return 0;
}