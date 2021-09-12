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
'''

def_re = re.compile(r'^(\S+)\s+(.*?)\((.*?)\)')
param_re = re.compile(r'^(.*?)\s*([^ &*]+)$')

info = []

for l in defs.strip().splitlines():
    grouped = re.search(def_re, l)
    return_type = grouped.group(1)
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

    for p in params:
        param_types.append(p[0])
        param_names.append(p[1])
        ipc_hook_args.append(f'        {{"{p[1]}", tostring({p[1]})}}')
    ipc_hook_args = ',\n'.join(ipc_hook_args)
    prototypes += '%s hooked_%s(%s);\n' % (return_type, name, ', '.join(param_types))
    hook_map += f'    {{ (PVOID)&{f[0]}, (PVOID)&hooked_{name} }},\n'

    # for p in params:
    # ipc_hook = f'''    compose_hook_message("{name}", {{
        
#     write_pipe(compose_hook_message("{name}", {{
# {ipc_hook_args}
#     }}));
    # }}\n'''
    if name == 'WriteFile':
        hook_funcs += f'''{return_type} hooked_{name}({params_line}) {{
    if (hFile != pipe)
    {{
        send_hook_message("{name}", {{
    {ipc_hook_args}
        }});
    }}
    decltype({name})* old_{name} = (decltype({name})*) yhook_map[{i}].original;
    return old_{name}({', '.join(param_names)});
}}
'''
    else:
        hook_funcs += f'''{return_type} hooked_{name}({params_line}) {{
    send_hook_message("{name}", {{
{ipc_hook_args}
    }});
    decltype({name})* old_{name} = (decltype({name})*) yhook_map[{i}].original;
    return old_{name}({', '.join(param_names)});
}}
'''


# hook = [
#     "MessageBox",
#     "CreateFile",
#     "WriteFile",
#     "ReadFile",
#     "HeapCreate",
#     "HeapDestroy",
#     "HeapFree",
#     "RegCreateKeyEx",
#     "RegSetValueEx",
#     "RegDeleteValue",
#     "RegCloseKey",
#     "RegOpenKeyEx",
#     "socket",
#     "bind",
#     "send",
#     "connect",
#     "recv"
# ]

# hook_map = ""
# hook_funcs = ""

# for i, v in enumerate(hook):
#     hook_map += f"    {{ (PVOID)&{v}, (PVOID)&hooked_{v} }},\n"
#     hook_funcs += f'''auto hooked_{v}() {{
#     decltype({v})& old_{v} = hook_map[{i}].original;
#     return old_{v}();
# }}

# '''

# generated = f'''
# hook_entry hook_map[] = {{
# { hook_map }
# }};

# {hook_funcs}
# '''

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

# with open(sys.argv[1], "w") as f:
#     f.write(generated)
