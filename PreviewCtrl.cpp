/**
 * 手动创建窗口程序
 * 
 * 编译：cl CreateWnd.cpp ManulConn_Wnd.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib
*/
#include "PreviewOnCtrl.h"
#include <Windows.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
#include <fstream>
using namespace std;

HWND hwnd;

IMediaEventEx *pEvent;
IMediaControl *pControl;

CButton * ctl_btn;

CStatic  * pPictureControl;

void ShowFilterInfo(IBaseFilter *filter)
{
    FILTER_INFO finfo;
    HRESULT hr = filter->QueryFilterInfo(&finfo);
    if (S_OK == hr)
    // cout << finfo.achName << endl;
    {
        CLSID clsid;
        filter->GetClassID(&clsid);

        cout << "filter class id " << hex << clsid.Data1 << "-" << clsid.Data2 << "-" << clsid.Data3 << endl;

        _tprintf(_T("filter name %ws\r\n"), finfo.achName);

        if (finfo.pGraph != NULL)
        {
            finfo.pGraph->Release();
        }
    }
    else
    {
        cout << "Show Filter Info Failed " << hr << endl;
    }
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    cout << "WndProc Msg " << msg << endl;

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        // nothing to do
        EndPaint(hwnd, &ps);
        break;

    case WM_FGNOTIFY:
        cout << "recv a filter graph event " << endl;
        // // uh-oh, something went wrong while capturing - the filtergraph
        // // will send us events like EC_COMPLETE, EC_USERABORT and the one
        // // we care about, EC_ERRORABORT.
        // // 获取filter graph manager发出的事件
        if (pEvent)
        {
            LONG event;
            LONG_PTR l1, l2;
            HRESULT hrAbort = S_OK;
            BOOL bAbort = FALSE;
            while (pEvent->GetEvent(&event, &l1, &l2, 0) == S_OK) // 循环读取filter graph manager事件队列
            {
                pEvent->FreeEventParams(event, l1, l2);
                if (event == EC_ERRORABORT) // filter graph退出/中断
                {
                    cout << "filter graph abort" << endl;
                    // StopCapture();
                    bAbort = TRUE;
                    hrAbort = static_cast<HRESULT>(l1);
                    continue;
                }
                else if (event == EC_DEVICE_LOST) // graph中的设备热插拔事件
                {
                    cout << "device lost event " << endl;

                    // Check if we have lost a capture filter being used.
                    // lParam2 of EC_DEVICE_LOST event == 1 indicates device added （1 设备插入）
                    //                                 == 0 indicates device removed （0 设备移除）
                    if (l2 == 0)
                    {
                        IBaseFilter *pf;
                        IUnknown *punk = (IUnknown *)l1;
                        if (S_OK == punk->QueryInterface(IID_IBaseFilter, (void **)&pf))
                        {
                            cout << "device lost " << endl;
                            ShowFilterInfo(pf);
                            pf->Release();
                        }
                    }
                }
                else
                {
                    cout << "other event " << dec << event << endl;
                }
            } // end while
        }
        break;
    }

    return (LONG)DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{

    static TCHAR szAppName[] = TEXT("HelloWin");
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    RegisterClass(&wndclass);

    hwnd = CreateWindow(szAppName,                 // window class name
                        TEXT("The Hello Program"), // window caption
                        WS_OVERLAPPEDWINDOW,       // window style
                        CW_USEDEFAULT,             // initial x position
                        CW_USEDEFAULT,             // initial y position
                        CW_USEDEFAULT,             // initial x size
                        CW_USEDEFAULT,             // initial y size
                        NULL,                      // parent window handle
                        NULL,                      // window menu handle
                        hInstance,                 // program instance handle
                        NULL);                     // creation parameters

CWnd* pWnd = CWnd::FromHandle(hwnd);// 通过对话框句柄获取 CWnd 对象

pPictureControl = new CStatic;
pPictureControl->Create(_T( "" ), WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect (10,10,259,154), pWnd, 12345);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // // show video
    // show();

    auto cbuf = cout.rdbuf();
    ofstream of1("log1.txt");
    auto fileBuf = of1.rdbuf();
    cout.rdbuf(fileBuf);

// hwnd = pPictureControl->GetSafeHwnd();

    // ShowVideo(hwnd, WM_FGNOTIFY, L"CX3-UVC", &pEvent, &pControl);
    ShowVideo(pPictureControl->GetSafeHwnd(), WM_FGNOTIFY, L"SnIris Camera", &pEvent, &pControl);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    of1.flush();
    of1.close();
    cout.rdbuf(cbuf);

    return msg.wParam;
}