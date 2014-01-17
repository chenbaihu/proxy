// Win32Project1.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <Richedit.h>
#include <process.h>
#include "resource.h"
#include "ObjectManager.h"
#include "commontypes.h"
#include "logger.h"



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
HWND re;

TCHAR szTitle[MAX_LOADSTRING] = TEXT("socksd server");					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING] = TEXT("socksd");			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void reportError(const char* input){
  if (input == NULL)
    return;
  int len = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
  if (len <= 0)
    return;
  WCHAR* ret = new WCHAR[len];
  len = MultiByteToWideChar(CP_UTF8, 0, input, -1, ret, len);
  if (len <= 0)
    return;
  MessageBox(NULL, ret, NULL, MB_OK | MB_ICONERROR);
}


unsigned APIENTRY service_thread(void* param){
  while (true){
    TCHAR buf[280];
    wsprintf(buf, L"test\n");
    CHARRANGE crg = { LONG_MAX, LONG_MAX };
    // Move the caret to the end
    SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&crg);
    SendMessage(re, EM_REPLACESEL, TRUE, (LPARAM)buf);
    SleepEx(100, FALSE);
  }
  return 0;
}


int run_window_loop(_In_ HINSTANCE hInstance, _In_ int       nCmdShow){
  //ret = my_main(argc+1, my_argv);    
  LoadLibrary(TEXT("Msftedit.dll"));

  // TODO: Place code here.
  MSG msg;
  HACCEL hAccelTable;


  if (MyRegisterClass(hInstance) == 0){
    std::string errstr = "register window class fail :" + slib::getErrorString(GetLastError());
    reportError(errstr.c_str());
    return -1;
  }

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow))
  {
    std::string errstr = slib::getErrorString(GetLastError());
    reportError(errstr.c_str());
    return -1;
  }
  unsigned threadID;
  HANDLE hThread=(HANDLE)_beginthreadex(NULL, 0, service_thread, NULL, 0, &threadID);

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
  // Main message loop:
  while (GetMessage(&msg, NULL, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  return (int)msg.wParam;

}
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);

  std::auto_ptr<slib::ObjectManager> om(slib::ObjectManager::instance());
  int argc;
  LPWSTR* wargv = CommandLineToArgvW(lpCmdLine, &argc);
  char** my_argv = new char*[argc + 2];
  my_argv[0] = _strdup("");
  for (int i = 0; i != argc; ++i){
    LPWSTR input = wargv[i];
    int newlen = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, NULL, NULL, NULL);
    if (newlen <= 0){
      return -1;
    }
    my_argv[i + 1] = new char[(size_t)(newlen + 1)];
    int len = WideCharToMultiByte(CP_UTF8, 0, input, -1, my_argv[i + 1], newlen, NULL, NULL);
    if (len <= 0) {
      return -1;
    }
  }
  my_argv[argc + 1] = NULL;
  int ret;
  try{
    ret = run_window_loop(hInstance, nCmdShow);
  } catch (std::exception& ex){
    ret = -1;
    reportError(ex.what());
  } catch (const char * str){
    ret = -1;
    reportError(str);
  }
  for (int i = 1; i != argc; ++i){
    delete my_argv[i];
  }
  delete[] my_argv;
  return ret;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;
  memset(&wcex, 0, sizeof(wcex));

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  //wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
  wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
  wcex.lpszClassName = szWindowClass;


  return RegisterClassEx(&wcex);
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
  HWND hWnd;

  hInst = hInstance; // Store instance handle in our global variable

  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, 800, 600, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  re = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("Hi!  Click anywhere on the bar above.\n"),
    WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER | WS_TABSTOP |
    ES_MULTILINE | ES_LEFT | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_READONLY,
    0, 0, 780, 540,
    hWnd, NULL, hInstance, NULL);
  if (!re)
    return FALSE;

  CHARFORMAT2 cf;
  cf.cbSize = sizeof(CHARFORMAT2);
  cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
  cf.crTextColor = RGB(255, 0, 0);
  cf.crBackColor = RGB(255, 255, 255);
  cf.dwEffects = CFE_BOLD;
  SendMessage(re, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

void OnCommand(HWND hWnd, int wmId, HWND wmEvent, UINT codeNotify){
  // Parse the menu selections:
  switch (wmId)
  {
  case IDM_ABOUT:
    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
    break;
  case IDM_EXIT:
    DestroyWindow(hWnd);
    break;
  default:
    break;
  }
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
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
