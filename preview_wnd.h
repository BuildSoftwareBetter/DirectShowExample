#include <afxwin.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
#include <fstream>
using namespace std;
extern HWND g_wnd_hwnd;

extern IMediaControl *pControl;
extern IMediaEventEx *pEvent;

// window messages
#define WM_FGNOTIFY WM_USER + 1

int show(HWND);

void ShowFilterInfo(IBaseFilter *filter);
void ShowGUID(GUID *guid);