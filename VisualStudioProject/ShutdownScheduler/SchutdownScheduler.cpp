// ShutdownScheduler.cpp
// This is a simple program to create a schduled shutdown, have a running timer, and prompt the user before shutting down
// The purpose of this is to teach myself the basics of programming for windows and to create a useful tool
// Most of this was taken from https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
// Programmer: Remington Ward
// Date: Feburary 25 2022

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
using namespace std;


//Defined below wWinMain this function is used to define various aspects of the window
//This is called by windows through messages 
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//HINSTANCE hInstance:		is used to identify an instance by windows OS
//HINSTANCE hPrevInstance:	not used since 16-bit windows, always set to 0
//PWSTR pCmdLine:			command line arguments as unicode string 
//int nCmdShow:				flag for is window is maximized, minimized, or normal
//Entry point for window
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    
    // First we must create a Window Class
    const wchar_t WINDOW_CLASS[] = L"Sample Window Class"; //String to identify the window class
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;        //Pointer to windows procedure that defines stuff for a window
    wc.hInstance = hInstance;           //Handle to the application instance given by wWinMain
    wc.lpszClassName = WINDOW_CLASS;    //String to identify the window class
    
    // Next we need to register our window class with the OS
    RegisterClass(&wc);

    // Then we can create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles. 0 gives defaults
        WINDOW_CLASS,                   // Window class
        L"Learn to Program Windows",    // Window text. Normally the title bar
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    //If hwnd is zero the window failed to create
    if (hwnd == NULL){
        return 0;
    }

    //This makes the window visible
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}








//int main() {
//	cout << "Schedule Shutdown [1] Cancel Shutdowns [2]" << endl;
//	char input;
//	cin >> input;
//	int hours;
//	int min;
//	if (input == '1') {
//		cout << "Hours until shutdown" << endl;
//		cin >> hours;
//		cout << "Minutes until shutdown" << endl;
//		cin >> min;
//		int seconds = hours*60*60 + min*60;
//		string secondsStr = to_string(seconds);
//		string command = "shutdown /s /t " + secondsStr;
//		char a[1000];
//		system(command.c_str());
//		cout << "Shutdown scheduled in " + secondsStr + " seconds. Or " + to_string(hours) + " hours, " + to_string(min) + " minutes." << endl;
//	}
//	else {
//		system("shutdown -a");
//		cout << "Canceled all shutdowns" << endl;
//	}
//	return 0;
//}