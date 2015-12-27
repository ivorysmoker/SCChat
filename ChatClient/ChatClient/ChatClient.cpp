// ChatClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ChatClient.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

using namespace std;

char sendbuf[256] = {0};
char recvbuf[256] = {0};
SOCKET sConnect;

//Hilfsvariable
long rc;

void TCharToChar(const wchar_t* Src, char* Dest, int Size)
{
	WideCharToMultiByte(CP_ACP, 0, Src, wcslen(Src) + 1, Dest, Size, NULL, NULL);
}


void MyThread() {
	long res;
	while (1) {
		memset(&recvbuf, 0, sizeof(recvbuf));
		res = recv(sConnect, recvbuf, 256, 0);
		if (res == SOCKET_ERROR) {
			cout << "error recv(): " << WSAGetLastError() << endl;
			cout << this_thread::get_id() << endl;
			break;
		}
		else {
			//cout << res << " bytes received: "
			//<< recvbuf << endl;
			cout << recvbuf << endl;
		}
	}
}

void Connection(HWND hWnd)
{

	WSADATA wsaData;
	sockaddr_in conpar;

	rc = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (rc == 0)
		cout << "WSAStartup()\t\t successful" << endl;
	else
		cout << "error WSAStartup(): " << WSAGetLastError() << endl;

	sConnect = socket(AF_INET, SOCK_STREAM, 0);
	if (sConnect != INVALID_SOCKET)
		cout << "socket()\t\t successful" << endl;
	else
		cout << "error socket(): " << WSAGetLastError() << endl;

	conpar.sin_addr.s_addr = inet_addr("178.82.150.109");
	conpar.sin_family = AF_INET;
	conpar.sin_port = htons(54345);
	int conparlen = sizeof(conpar);

	rc = connect(sConnect, (struct sockaddr*)&conpar, conparlen);
	if (rc != SOCKET_ERROR) {
		MessageBox(hWnd, TEXT("connect(): " + GetLastError()), NULL, MB_OK);
		thread Thread1(MyThread);
		Thread1.detach();
	}
	//cout << "connect()\t\t successful" << endl;
	else
		MessageBox(hWnd, TEXT("error connect(): " + GetLastError()), NULL, MB_OK);
	//cout << "error connect(): " << WSAGetLastError() << endl;

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHATCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHATCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

//Referenz zu den Child-Fenstern
static HWND textBoxInput;
static HWND textBoxMassage;

//Erstelle einen buffer für die Nachrichten im Edit Feld
TCHAR buffer[256] = {0};

int textHeight = 0;

HWND Parente;

//vector<string> test;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		Connection(hWnd);
		textBoxMassage = CreateWindowEx(NULL, L"Message", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, hWnd, NULL, hInst, NULL);
		if (textBoxMassage == NULL) {
			MessageBox(hWnd, TEXT("ERROR: CreateWindowsEx()" + GetLastError()), NULL, MB_OK);
		}
		textBoxInput = CreateWindowEx(NULL, L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 0, 400, 500, 20, hWnd, NULL, hInst, NULL);
		if (textBoxInput == NULL) {
			MessageBox(hWnd, TEXT("CreateWindowsEx(): " + GetLastError()), NULL, MB_OK);
		}
		HWND hWndButton = CreateWindowEx(NULL,
				L"BUTTON",
				L"Send",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				50,
				600,
				100,
				24,
				hWnd,
				(HMENU)IDC_MAIN_BUTTON,
				GetModuleHandle(NULL),
				NULL);
		//SendMessage(hWnd, textBoxInput, NULL, NULL);
		break;
	}
	/*case WM_KEYDOWN:
	{
		switch (wParam) {
		case VK_LEFT:
		{
			::MessageBox(hWnd, _T("Left Arrow"), _T("Key Pressed"), MB_OK);
			break;
		}
		case 'X':
		{
			::MessageBox(hWnd, _T("X"), _T("Key Pressed"), MB_OK);
			break;
		}
		}
		break;
	}*/
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDC_MAIN_BUTTON:
			{

				//MessageBox(hWnd, TEXT("Button clicked"), NULL, MB_OK);
				
				memset(buffer, 0, 256);
				//Holt den EDIT Text und schreibt es in den Buffer
				GetWindowText(textBoxInput, buffer, 256);
				//Setzt den Title des Hauptfensters 
				int success = SetWindowText(hWnd, buffer);
				if (success == NULL) {
					MessageBox(hWnd, TEXT("SetWindowText(): Error"), NULL, MB_OK);
				}
				RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
				textHeight = textHeight + 20;
				// TODO: Add any drawing code that uses hdc here...

				//Convert TCHAR to Char
				//const TCHAR TChrData[] = _T("Hello World");
				char AnsiBuffer[255];
				TCharToChar(buffer, AnsiBuffer, sizeof(AnsiBuffer));

				//Send Massage to Server
				rc = send(sConnect, AnsiBuffer, strlen(AnsiBuffer), 0);
				if (rc == SOCKET_ERROR) {
					MessageBox(hWnd, TEXT("SetWindowText(): Error"), NULL, MB_OK);
				}

				//char text[] = "Test";
				//TextOut(hdc, 10, 30, TEXT("lol"), strlen("lol"));
				//MessageBox(hWnd, buffer, NULL, MB_OK); // gibt die edit eingabe aus
				//SetWindowText(textBoxMassage, buffer); ändert den titel
				//Löse WM_PAINT aus... ( Dies passiert sonst erst sobald das Fenster sich bewegt und neugezeichnet werden muss...
				break;
			}
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			//char text[] = "Test";
			TextOut(hdc, 10, textHeight, buffer, sizeof(buffer));


			//MessageBox(NULL, szName, szName, MB_OK);
				//Convert CHAR TO TCHAR
				TCHAR textBuffer[256];
				wcstombs(recvbuf, textBuffer, sizeof(textBuffer));
				//MessageBox(NULL, textBuffer, textBuffer, MB_OK);

				//NULLL

				TextOut(hdc, 50, textHeight, textBuffer, sizeof(textBuffer));
			//TextOut(hdc, 10, 30, TEXT("lol"), strlen("lol"));
			EndPaint(hWnd, &ps);

			/*HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);*/
				/*PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code that uses hdc here...
				//char text[] = "Test";
				//TextOut(hdc, 10, 10, buffer, sizeof(buffer));
				//TextOut(hdc, 10, 30, TEXT("lol"), strlen("lol"));
				EndPaint(hWnd, &ps);*/
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
		closesocket(sConnect);
		WSACleanup();
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
