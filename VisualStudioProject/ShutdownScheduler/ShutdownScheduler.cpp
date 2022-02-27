// ShutdownScheduler.cpp
// This is a simple program to create a schduled shutdown, have a running timer, and prompt the user before shutting down
// The purpose of this is to teach myself the basics of programming for windows and to create a useful tool
// Most of this was taken from https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
// Also found this useful http://bit.ly/31MfGww
// Programmer: Remington Ward
// Date: Feburary 25 2022


#include <windows.h>
#include <winerror.h>
#include <string>
using namespace std;

//Reference to the main window's identifier
HWND mainWindow;

//This is the syntax for a thread's function
DWORD WINAPI TimerThread(_In_ LPVOID lpParameter);

//Checks valididty of input for hours, min, and seconds
bool SetTime(char* hourptr, char* minptr, char*secptr);

//Processes messages sent to the window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Entry point for windows applications
//PARAMS:
// HINSTANCE hInstance:      is used to identify an instance by windows OS
// HINSTANCE hPrevInstance:  not used since 16-bit windows, always set to 0
// PWSTR pCmdLine:           command line arguments as unicode string 
// int nCmdShow:             flag for is window is maximized, minimized, or normal
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
        L"Remi's ShutdownScheduler",    // Window text. Normally the title bar
        WS_SYSMENU | WS_MINIMIZEBOX,    // Window style

        CW_USEDEFAULT, CW_USEDEFAULT,   // Use default screen position
        500, 500,                       // size width, hieght in pixels

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
    //------------------------------------------------------------------------------------------------
    // GetMessage explanation
    // GetMessage gets a message from the queue associated with window messages for our thread
    // GetMessage's NULL and 0 parameters below are used to filter types of messages but generally we set them to NULL or 0 like below
    // If no message is pressent the program blocks until one is present which is normally fine as your program has nothing to do. 
    // If you need to do background processing at the same time you can create additional threads
    // GetMessage will return true unless the quit message is sent
    //------------------------------------------------------------------------------------------------

    MSG msg = { }; // stores a message to the window
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg); //Call before dispatch. Translates keyboard input to characters
        DispatchMessage(&msg);  //Tells OS to call window prcedure AKA WindowProc for this window
    }

	return 0;
}

const int ID_BUTTON_SUBMIT = 1;
const int ID_BUTTON_CANCEL = 2;
bool runTimer = false;
HWND hwndHours;
HWND hwndMinutes;
HWND hwndSeconds;
int timeLeftSeconds = 0;

//Where messages from the OS are recieved detailing events
//PARAMS:
// hwnd:    Handle to the window
// uMsg:    Message code
// wParam:  Data pertaining to message
// lParam:  Data pertaining to message
// No new messages can be processed while this runs. Consider using another thread for lengthy processes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        //Creating window
        case WM_CREATE: {
            // --------------- HOURS -----------------
            //Create text
            CreateWindow(
                TEXT("STATIC"),                 //Type of window
                TEXT("Hours"),     //Text
                WS_VISIBLE | WS_CHILD,          //Flags
                10, 10, 100, 40,                  //Position and size. (x,y,width,height)
                hwnd,                           //Parent window 
                (HMENU)NULL,                    //ID
                NULL, NULL                      //Always null I guess?
            );
            //Create textBox
            hwndHours = CreateWindow(
                TEXT("EDIT"),                       //Type of window
                TEXT("0"),                          //Text
                WS_VISIBLE | WS_CHILD | WS_BORDER,  //Flags
                10, 50, 100, 25,                    //Position and size. (x,y,width,height)
                hwnd,                               //Parent window 
                (HMENU)NULL,               //ID
                NULL, NULL                          //Always null I guess?
            );

            // --------------- Minutes -----------------
            //Create text
            CreateWindow(
                TEXT("STATIC"),                 //Type of window
                TEXT("Minutes"),                //Text
                WS_VISIBLE | WS_CHILD,          //Flags
                120, 10, 100, 40,               //Position and size. (x,y,width,height)
                hwnd,                           //Parent window 
                (HMENU)NULL,                    //ID
                NULL, NULL                      //Always null I guess?
            );
            //Create textBox
            hwndMinutes = CreateWindow(
                TEXT("EDIT"),                       //Type of window
                TEXT("0"),                          //Text
                WS_VISIBLE | WS_CHILD | WS_BORDER,  //Flags
                120, 50, 100, 25,                   //Position and size. (x,y,width,height)
                hwnd,                               //Parent window 
                (HMENU)NULL,             //ID
                NULL, NULL                          //Always null I guess?
            );

            // --------------- SECONDS -----------------
            //Create text
            CreateWindow(
                TEXT("STATIC"),         //Type of window
                TEXT("Seconds"),        //Text
                WS_VISIBLE | WS_CHILD,  //Flags
                230, 10, 100, 40,       //Position and size. (x,y,width,height)
                hwnd,                   //Parent window 
                (HMENU)NULL,            //ID
                NULL, NULL              //Always null I guess?
            );
            //Create textBox
            hwndSeconds = CreateWindow(
                TEXT("EDIT"),                       //Type of window
                TEXT("0"),                          //Text
                WS_VISIBLE | WS_CHILD | WS_BORDER,  //Flags
                230, 50, 100, 25,                   //Position and size. (x,y,width,height)
                hwnd,                               //Parent window 
                (HMENU)NULL,             //ID
                NULL, NULL                          //Always null I guess?
            );

            // --------------- SUBMIT -----------------
            //Create submit button
            CreateWindow(
                TEXT("BUTTON"),                 //Type of window
                TEXT("Start"),      //Text for button
                WS_VISIBLE | WS_CHILD,          //Flags
                10, 100, 150, 50,               //Position and size. (x,y,width,height)
                hwnd,                           //Parent window 
                (HMENU)ID_BUTTON_SUBMIT,        //ID
                NULL, NULL                      //Always null I guess?
            );

            // --------------- CANCEL -----------------
            //Create cancel button
            CreateWindow(
                TEXT("BUTTON"),                 //Type of window
                TEXT("Stop"),      //Text for button
                WS_VISIBLE | WS_CHILD,          //Flags
                180, 100, 150, 50,               //Position and size. (x,y,width,height)
                hwnd,                           //Parent window 
                (HMENU)ID_BUTTON_CANCEL,        //ID
                NULL, NULL                      //Always null I guess?
            );


            return 0;
        }
        //This is where our program recieve commands from buttons and actions
        case WM_COMMAND: {
            //If our button is pressed
            switch (LOWORD(wParam))
            {
                case ID_BUTTON_SUBMIT: {
                    char hourCSTR[20]; 
                    char minCSTR[20]; 
                    char secCSTR[20];
                    GetWindowTextA(hwndHours, hourCSTR, 10);
                    GetWindowTextA(hwndMinutes, minCSTR, 10);
                    GetWindowTextA(hwndSeconds, secCSTR, 10); 
                    if (SetTime(hourCSTR, minCSTR, secCSTR)) {
                        runTimer = true;
                        CreateThread(NULL, 0, TimerThread, NULL, 0, NULL);
                    }
                    else {
                        MessageBoxA(hwnd, "Invalid time", "TITLE", MB_OK);
                    }
                    break;
                }
                case ID_BUTTON_CANCEL: {
                    runTimer = false;
                    break;
                }
                default:
                    break;
            }
            return 0;
        }

        case WM_CLOSE: //Top right X botton clicked
            DestroyWindow(hwnd);//Starts to destroy window and sends WM_DESTROY message
            return 0;

        case WM_DESTROY: // When window starts to be destroyed
            PostQuitMessage(0); //Puts a message on the queue that tells the program to quit. Makes GetMessage return false
            return 0;

        case WM_PAINT: //We need to paint the client area of the window
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
    
            //Between BeginPaint and EndPaint we do our stuff
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //DefWindowProc tells windows to do its default stuff for a particular message
}

bool SetTime(char* hourstr, char* minstr, char* secstr) {
    
    for (size_t i = 0; i < strlen(hourstr); i++){
        if (!isdigit(hourstr[i]))
            return false;
    }
    for (size_t i = 0; i < strlen(minstr); i++) {
        if (!isdigit(minstr[i]))
            return false;
    }
    for (size_t i = 0; i < strlen(secstr); i++) {
        if (!isdigit(secstr[i]))
            return false;
    }
    int hourInt = atoi(hourstr);
    int minInt = atoi(minstr);
    int secInt = atoi(secstr);
    timeLeftSeconds = secInt + (minInt * 60) + (hourInt * 60 * 60);

    return true;
}

//Used by the timer thread
DWORD WINAPI TimerThread(_In_ LPVOID lpParameter){
    while (runTimer) {
        if (timeLeftSeconds == 0) {
            MessageBeep(3);
            system("shutdown -s -t 15");
            if (MessageBox(mainWindow, L"Shutting Down . . .", L"Scheduled Shutdown", MB_OKCANCEL) == IDCANCEL) {
                system("shutdown /a");
            }
            return 0;
        }
        Sleep(1000);
        timeLeftSeconds--;
        int hours = timeLeftSeconds / 60 / 60;
        int min = timeLeftSeconds / 60 % 60;
        int sec = timeLeftSeconds % 60 % 60;
        SetWindowTextA(hwndHours, to_string(hours).c_str());
        SetWindowTextA(hwndMinutes, to_string(min).c_str());
        SetWindowTextA(hwndSeconds, to_string(sec).c_str());
    }
}