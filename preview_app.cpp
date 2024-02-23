// #include <windows.h>
#include "preview_wnd.h"

HWND g_wnd_hwnd;

/*----------------------------------------------------------------------------*\
|   AppWndProc( hwnd, uiMessage, wParam, lParam )                              |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app's main (tiled) window.  This processes all |
|       of the parent window's messages.                                       |
|                                                                              |
|   Arguments:                                                                 |
|       hwnd            window handle for the window                           |
|       msg             message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    int cxBorder, cyBorder, cy;

    switch (msg)
    {
    case WM_CREATE:
        break;

    case WM_COMMAND:
        break;

    case WM_INITMENU:
        break;

    case WM_INITMENUPOPUP:
        break;

    //
    // We're out of here!
    //
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CLOSE:
        break;

    case WM_ENDSESSION:

        break;

    case WM_ERASEBKGND:
        break;

    // ESC will stop capture
    case WM_KEYDOWN:

        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        // nothing to do
        EndPaint(hwnd, &ps);
        break;

    case WM_TIMER:

        break;

    case WM_SIZE:

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
                cout << "get event " << event << endl;
                cout << "get event " << dec << event << endl;

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
                else if (event == EC_PAUSED) // graph中的设备热插拔事件
                {
                    pControl->Run();
                }
                else
                {
                    cout << "other event " << dec << event << endl;
                }
            } // end while
        }
        break;

    case WM_DEVICECHANGE:
        // // We are interested in only device arrival & removal events
        // // 注册的感兴趣的设备热插拔事件
        // if (DBT_DEVICEARRIVAL != wParam && DBT_DEVICEREMOVECOMPLETE != wParam)
        //     break;

        // PDEV_BROADCAST_HDR pdbh = (PDEV_BROADCAST_HDR)lParam;
        // if (pdbh->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE)
        // {
        //     break;
        // }

        // PDEV_BROADCAST_DEVICEINTERFACE pdbi = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
        // // Check for capture devices.
        // if (pdbi->dbcc_classguid != AM_KSCATEGORY_CAPTURE)
        // {
        //     break;
        // }

        // // Check for device arrival/removal.
        // if (DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam)
        // {
        //     gcap.fDeviceMenuPopulated = false;
        // }
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

    g_wnd_hwnd = CreateWindow(szAppName,                 // window class name
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

    ShowWindow(g_wnd_hwnd, iCmdShow);
    UpdateWindow(g_wnd_hwnd);

    auto cbuf = cout.rdbuf();

    ofstream of1("log.txt");
    auto fileBuf = of1.rdbuf();

    cout.rdbuf(fileBuf);

    cout << "start show " << endl;
    // const char *path = "log.txt";
    // //   sprintf(path, "log.txt", m_index, index++);
    // FILE *file = fopen((const char *)path, "w");
    // const char *log = "start show";
    // fwrite(log, strlen(log), 1, file);
    // fclose(file);

    // show video
    show(g_wnd_hwnd);

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