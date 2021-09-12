import sys
import re

defs = '''
int         MessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
HANDLE      CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
WINBOOL     WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
WINBOOL     ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
HANDLE      HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
WINBOOL     HeapDestroy(HANDLE hHeap)
WINBOOL     HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
LONG        RegCreateKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
LONG        RegSetValueEx(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData)
LONG        RegDeleteValue(HKEY hKey, LPCWSTR lpValueName)
LONG        RegCloseKey(HKEY hKey)
LONG        RegOpenKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
SOCKET      socket(int af, int type, int protocol)
int         bind(SOCKET s, const sockaddr *name, int namelen)
int         send(SOCKET s, const char *buf, int len, int flags)
int         connect(SOCKET s, const sockaddr *name, int namelen)
int         recv(SOCKET s, char *buf, int len, int flags)
void * memcpy(void * __restrict__ __dst, const void * __restrict__ __src, size_t __n)
'''

def_re = re.compile(r'^\s*(.+?)\s*([^ &*]+)\((.*?)\)')
param_re = re.compile(r'^(.*?)\s*([^ &*]+)$')

info = []

for l in defs.strip().splitlines():
    grouped = re.search(def_re, l)
    return_type = grouped.group(1).strip()
    name = grouped.group(2)
    params = []
    params_line = grouped.group(3)
    for param_pair in params_line.split(','):
        param_pair = param_pair.strip()
        param_grouped = re.search(param_re, param_pair)
        param_type = param_grouped.group(1)
        param_name = param_grouped.group(2)
        # print(param_type, "->", param_name)
        params.append((param_type, param_name))
    info.append((name, return_type, params, params_line))

func_options = {
    "memcpy": {
        "override_args": [
            ("src", "std::string((char *)__src, __n)"),
            ("dst", "std::string((char *)__dst, __n)")
        ],
        "prevent_recursion": True
    },
    "send": {
        "add_args": [
            ("data", "std::string(buf, len)")
        ]
    },
    "recv": {
        "add_args": [
            ("data", "std::string(buf, len)")
        ]
    }
}

prototypes = ''
hook_map = ''
hook_funcs = ''

for i, f in enumerate(info):
    name = f[0]
    return_type = f[1]
    params = f[2]
    params_line = f[3]
    param_types = []
    param_names = []

    ipc_hook_args = []
    options = func_options[name] if name in func_options else {}

    override_args = options["override_args"] if "override_args" in options else []
    add_args = options["add_args"] if "add_args" in options else []
    del_args = options["del_args"] if "del_args" in options else []

    for p in params:
        param_types.append(p[0])
        param_names.append(p[1])

    if override_args:
        for p in override_args:
            ipc_hook_args.append(f'        {{"{p[0]}", {p[1]}}}')
    else:
        for p in params:
            if p[1] not in del_args:
                ipc_hook_args.append(f'        {{"{p[1]}", tostring({p[1]})}}')
        for p in add_args:
            ipc_hook_args.append(f'        {{"{p[0]}", {p[1]}}}')

    ipc_hook_args = ',\n'.join(ipc_hook_args)
    prototypes += '%s hooked_%s(%s);\n' % (return_type, name, ', '.join(param_types))
    hook_map += f'    {{ (PVOID)&{f[0]}, (PVOID)&hooked_{name} }},\n'

    prevent_recursion = options["prevent_recursion"] if "prevent_recursion" in options else False

    if prevent_recursion:
        hook_funcs += f'''{return_type} hooked_{name}({params_line}) {{
    static bool entered = false;
    if (!entered)
    {{
        entered = true;
        {options["func_enter"] if "func_enter" in options else ""}
        send_hook_message("{name}", {{
    {ipc_hook_args}
        }});
        entered = false;
    }}
    decltype({name})* old_{name} = (decltype({name})*) yhook_map[{i}].original;
    {return_type} ret = old_{name}({', '.join(param_names)});
    {options["func_leave"] if "func_leave" in options else ""}
}}
'''
    else:
        hook_funcs += f'''{return_type} hooked_{name}({params_line}) {{
    {options["func_enter"] if "func_enter" in options else ""}
    send_hook_message("{name}", {{
{ipc_hook_args}
    }});
    decltype({name})* old_{name} = (decltype({name})*) yhook_map[{i}].original;
    {return_type} ret = old_{name}({', '.join(param_names)});
    {options["func_leave"] if "func_leave" in options else ""}
}}
'''

generated = f'''#include <string>
#include <sstream>
#include "yhook.h"
#include "ystr.h"

{ prototypes }
yhook_entry yhook_map[] = {{
{ hook_map }}};
constexpr size_t yhook_map_length = sizeof(yhook_map) / sizeof(yhook_map[0]);

{ hook_funcs }'''

print(generated)