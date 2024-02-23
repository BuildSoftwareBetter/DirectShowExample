#include <afxwin.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
#include <fstream>
using namespace std;
extern HWND hwnd;

extern IMediaControl *pControl;
extern IMediaEventEx *pEvent;

// window messages
#define WM_FGNOTIFY WM_USER + 1

int ShowVideo(HWND hwnd, long lMsg, BSTR szVideoDeviceName, IMediaEventEx **opEvent, IMediaControl **opControl);

void ShowFilterInfo(IBaseFilter *filter);
void ShowGUID(GUID *guid);