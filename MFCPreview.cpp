#include "MFCPreview.h"
// #include <Qedit.h>

extern HWND hwnd;

BOOL ChooseDevice(TCHAR *szDeviceName, IBaseFilter **pFilter)
{
    HRESULT hr;
    ICreateDevEnum *pSysDevEnum = NULL;
    // 创建并默认初始化一个指定CLSID关联的类的对象
    // 创建SystemDeviceEnum对象
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
    IEnumMoniker *pEnumCat = NULL;
    // 创建关联的设备类型枚举对象，枚举视频输入设备
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

    IMoniker *pMoniker = NULL;
    ULONG cFected;
    // 循环获取设备
    while (pEnumCat->Next(1, &pMoniker, &cFected) == S_OK)
    {
        IPropertyBag *pPropBag = NULL;
        // 获取设备信息
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);

        if (SUCCEEDED(hr))
        {
            // To retrieve the filter's friendly name, do the following:
            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            if (SUCCEEDED(hr))
            {
                // Display the name in your UI somehow.
                printf("%S\n", varName.bstrVal);
            }
            VariantClear(&varName);

            // 获取封装设备的Filter
            hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)pFilter);

            pPropBag->Release();

            break;
        }
    }

    pEnumCat->Release();
    pSysDevEnum->Release();

    return TRUE;
}

int show()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        // init COM failed
        cout << "init COM failed" << endl;
        return 0;
    }

    // 创建Filter Graph Manager对象
    IGraphBuilder *pGraph;
    // 查找类标识为CLSID_FilterGraph的COM组件，使用改组件创建接口标识符为IID_IGraphBuilder的进程内组件对象pGraph
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        // 创建 Filter Graph
        return 0;
    }

    /** 
     * 获取Filter Graph Manager的两个接口
     * 1. IMediaControl 控制filter graph的启动和停止
     * 2. IMediaEvent 从Filter Graph Manager获取事件，例如视频文件播放完毕事件
     * */

    IMediaControl *pControl;
    IMediaEvent *pEvent;
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

    ICaptureGraphBuilder2 *pBuild = NULL;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&pBuild);
    if (FAILED(hr))
        return 0;

    pBuild->SetFiltergraph(pGraph);

    // build graph
    IBaseFilter *pDeviceFilter = NULL;

    ChooseDevice(_T("SnIris Camera"), &pDeviceFilter);

    cout << hex << pDeviceFilter << endl;

    pGraph->AddFilter(pDeviceFilter, L"SnIris Camera");

    IBaseFilter *pSmartTee = NULL;
    // // 添加smart tee ， 分成两路，一路预览，一路抓图
    hr = CoCreateInstance(CLSID_SmartTee, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pSmartTee);
     cout << " Create Smart Tee " << hex << hr << endl;
    pGraph->AddFilter(pDeviceFilter, L"Smart Tee");

    // grabber sample
    // hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
    // 	IID_IBaseFilter, (LPVOID *)&m_pSampleGrabberFilter);

    IBaseFilter *pNullFilter = NULL;
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&pNullFilter);

    hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pDeviceFilter, NULL, NULL);
    // hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pDeviceFilter, NULL, pSmartTee);
    cout << " RenderStream 1 " << hex << hr << endl;

    // hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pSmartTee, NULL, NULL);
    // cout << " RenderStream 2 " << hex << hr << endl;
    // hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSmartTee, NULL, pNullFilter);
    // cout << " RenderStream 3 " << hex << hr << endl;

    // if (hr != S_OK && hr != VFW_S_NOPREVIEWPIN)
    // {
    //     cout << "render MEDIATYPE_Video stream failed : " << hex << hr << endl;
    //     hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved, pDeviceFilter, NULL, NULL);

    //     if (hr == VFW_S_NOPREVIEWPIN)
    //     {
    //         cout << "use capture pin" << endl;
    //     }
    //     else if (hr != S_OK)
    //     {
    //         cout << "render MEDIATYPE_Interleaved stream failed : " << hex << hr << endl;
    //     }
    // }

    IVideoWindow *pVidWin = NULL;
    pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVidWin);
    pVidWin->put_Owner((OAHWND)hwnd);
    pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    RECT rc;
    GetClientRect(hwnd, &rc);
    pVidWin->SetWindowPosition(0, 0, rc.right, rc.bottom);

    hr = pControl->Run();

    cout << "start run : " << hr << endl;

    // // if (hr == S_OK)
    // Sleep(1000 * 100);

    // cout << "complete" << endl;

    // pControl->Stop();

    // pControl->Release();
    // pEvent->Release();
    // pGraph->Release();
    // CoUninitialize();

    return 0;
}