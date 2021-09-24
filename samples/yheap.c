#include <windows.h>

#define PAGE_SIZE 1024

int main()
{
    HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE * 10, PAGE_SIZE * 100);

    int *pArr = (int *)HeapAlloc(hHeap, 0, sizeof(int) * 30);

    for (int i = 0; i < 10; ++i)
    {
        if (i % 5 == 0)
            printf_s("\n");
        pArr[i] = (i + 1) * 100;
    }

    for (int i = 0; i < 30; ++i)
    {
        if (i % 5 == 0)
            printf_s("\n");
        printf_s("%d ", pArr[i]);
    }
    HeapFree(hHeap, 0, pArr);
    HeapFree(hHeap, 0, pArr);
    HeapDestroy(hHeap);

    getchar();

    return 0;
}