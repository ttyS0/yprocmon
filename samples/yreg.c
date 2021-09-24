#include <windows.h>

BOOL CreateRegistryKey(HKEY hKeyParent, PTCHAR subkey)
{
    DWORD dwDisposition; // It verify new key is created or open existing key
    HKEY hKey;
    DWORD Ret;
    Ret = RegCreateKeyEx(hKeyParent, subkey, 0, NULL, REG_OPTION_NON_VOLATILE,
                         KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
    if (Ret != ERROR_SUCCESS)
    {
        printf("Error opening or creating new key\n");
        return FALSE;
    }
    RegCloseKey(hKey); // close the key
    return TRUE;
}

int main()
{
    // CreateRegistryKey(HKEY_CURRENT_USER, TEXT("SOFTWARE\\SHOULD_ALERT"));
    DWORD return_val;
    HKEY hKey;
    return_val = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0,
        KEY_ALL_ACCESS | KEY_EXECUTE, &hKey);

    return_val = RegSetValueEx(
        hKey, (LPCWSTR) "Writing to the Registry Example", 0, REG_SZ,
        TEXT("C:\\a.exe"), 9);

    RegCloseKey(hKey);

    return 0;
}