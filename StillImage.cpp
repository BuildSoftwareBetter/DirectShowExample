/**
 * 使用still pin手动拍照
*/
#include <windows.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
#include "SampleGrabber.h"
#include "SaveGraphFile.h"

using namespace std;

void ShowGUID(GUID *guid)
{
    cout << hex << guid->Data1 << "-" << guid->Data2 << "-" << guid->Data3 << "-";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << guid->Data4[i];
    }
    cout << endl;
}

void ShowPinInfo(IPin *pin)
{
    PIN_INFO info;
    HRESULT hr = pin->QueryPinInfo(&info);
    if (S_OK == hr)
    {
        setlocale(LC_ALL, "chs");
        _tprintf(_T("pin name %ls\t"), info.achName);
        _tprintf(_T("pin name %ws\t"), info.achName);
        _tprintf(_T("direction %d\r\n"), info.dir);
        if (NULL != info.pFilter)
        {
            // PIN_DIRECTION::PINDIR_OUTPUT
            // ShowFilterInfo(info.pFilter);
            info.pFilter->Release();
        }
    }
    else
    {
        cout << "get pin info failed" << endl;
    }
}

GUID GetPinCategory(IPin *pPin)
{
    IKsPropertySet *pKs = NULL;
    HRESULT hr = pPin->QueryInterface(IID_PPV_ARGS(&pKs));
    if (SUCCEEDED(hr))
    {
        GUID PinCategory;
        DWORD cbReturned;
        hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,
                      &PinCategory, sizeof(GUID), &cbReturned);

        pKs->Release();
        if (SUCCEEDED(hr) && (cbReturned == sizeof(GUID)))
        {
            return PinCategory;
        }
    }

    return {0, 0, 0, 0};
}

BOOL PinMatchesCategory(IPin *pPin, REFGUID Category)
{
    BOOL bFound = FALSE;

    IKsPropertySet *pKs = NULL;
    HRESULT hr = pPin->QueryInterface(IID_PPV_ARGS(&pKs));
    if (SUCCEEDED(hr))
    {
        GUID PinCategory;
        DWORD cbReturned;
        hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,
                      &PinCategory, sizeof(GUID), &cbReturned);
        if (SUCCEEDED(hr) && (cbReturned == sizeof(GUID)))
        {
            bFound = (PinCategory == Category);
        }
        pKs->Release();
    }
    return bFound;
}

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

int main()
{
    HRESULT hr = CoInitialize(NULL);

    // 1. 构建filter graph
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
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    // 2. 找到设备，绑定filter
    IBaseFilter *pDeviceFilter = NULL;
    BOOL br = ChooseDevice(_T("UVC Camera"), &pDeviceFilter);
    cout << dec << __LINE__ << '\t' << hex << hr << '\t' << pDeviceFilter << endl;

    IPin *pSpin = NULL;

    IEnumPins *pDeviceEnumPins = NULL;
    // 设置输出视频格式及参数
    hr = pDeviceFilter->EnumPins(&pDeviceEnumPins);
    IPin *pDeviceOutPin = NULL;
    while (pDeviceEnumPins->Next(1, &pDeviceOutPin, NULL) == S_OK)
    {
        ShowPinInfo(pDeviceOutPin);
        GUID guid = GetPinCategory(pDeviceOutPin);
        ShowGUID(&guid);

        if (guid == PIN_CATEGORY_STILL)
            pSpin = pDeviceOutPin;
        else
            pDeviceOutPin->Release();
    }
    pDeviceEnumPins->Release();

    // 3. 创建sample grabber filter
    IBaseFilter *pGrabberFilter = NULL;
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pGrabberFilter);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;

    // 7. 为sample grabber设置回调接口对象
    ISampleGrabber *pGrabber = NULL;
    hr = pGrabberFilter->QueryInterface(IID_ISampleGrabber, (void **)&pGrabber);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    hr = pGrabber->SetOneShot(FALSE);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    hr = pGrabber->SetBufferSamples(FALSE);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    CSampleGrabberCB *scb = new CSampleGrabberCB;
    hr = pGrabber->SetCallback(scb, 1);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;

    // 4. 创建Null Renderer filter
    IBaseFilter *pNullFilter = NULL;
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&pNullFilter);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    // 5. 将三个filter添加到filter graph

    // 6. 找到各个filter的pin，将设备filter的still pin与sample filter的输入pin连接，将sample filter的输出pin与Null Renderer的输入pin连接

    /**
     * 更方便的方式是使用 ICaptureGraphBuilder2
    */
    ICaptureGraphBuilder2 *pBuild = NULL;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&pBuild);
    if (FAILED(hr))
        return 0;

    hr = pBuild->SetFiltergraph(pGraph);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;

    hr = pGraph->AddFilter(pDeviceFilter, L"device filter");
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    hr = pGraph->AddFilter(pGrabberFilter, L"sample grabber filter");
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    hr = pGraph->AddFilter(pNullFilter, L"null renderer filter");
    cout << dec << __LINE__ << '\t' << hex << hr << endl;

    IPin *pStillPin = NULL;
    hr = pBuild->FindPin(pDeviceFilter, PIN_DIRECTION::PINDIR_OUTPUT, &PIN_CATEGORY_PREVIEW, NULL, FALSE, 1, (IPin **)&pStillPin);
    if (hr == S_OK)
        pStillPin->Release();
    hr = pBuild->FindPin(pDeviceFilter, PIN_DIRECTION::PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, NULL, FALSE, 1, (IPin **)&pStillPin);
    if (hr == S_OK)
        pStillPin->Release();
    hr = pBuild->FindPin(pDeviceFilter, PIN_DIRECTION::PINDIR_OUTPUT, &PIN_CATEGORY_STILL, NULL, FALSE, 1, (IPin **)&pStillPin);
    if (hr == S_OK)
        pStillPin->Release();

    hr = pBuild->FindPin(pDeviceFilter, PIN_DIRECTION::PINDIR_OUTPUT, NULL, NULL, FALSE, 1, (IPin **)&pStillPin);
    if (hr == S_OK)
        pStillPin->Release();

    hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pDeviceFilter, NULL, NULL);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;

    IEnumPins *pEnumPins = NULL;
    IPin *pPin = NULL;
    IPin *pSGInPin = NULL;
    IPin *pSGOutPin = NULL;
    hr = pGrabberFilter->EnumPins(&pEnumPins);
    while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION pdir;
        pPin->QueryDirection(&pdir);
        if (pdir == PIN_DIRECTION::PINDIR_INPUT)
            pSGInPin = pPin;
        else
            pSGOutPin = pPin;
    }
    pEnumPins->Release();

    hr = pNullFilter->EnumPins(&pEnumPins);
    while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION pdir;
        pPin->QueryDirection(&pdir);
        if (pdir == PIN_DIRECTION::PINDIR_INPUT)
            break;
        pPin->Release();
    }
    pEnumPins->Release();

    // pSpin->Connect(pSGInPin,);
    hr = pGraph->Connect(pSpin, pSGInPin);
    hr = pGraph->Connect(pSGOutPin, pPin);
    // pGraph->Connect()

    // hr = pBuild->RenderStream(NULL, NULL, pGrabberFilter, NULL, pNullFilter);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    if (hr == E_FAIL)
    {
        auto err = GetLastError();
        cout << "last error " << err << endl;
    }

    SaveGraphFile(pGraph, L"still.grf");

    AM_MEDIA_TYPE mediaType;
    hr = pGrabber->GetConnectedMediaType(&mediaType);
    ShowGUID(&mediaType.subtype);
    ShowGUID(&mediaType.formattype);

    // 8. 获取设备IID_IAMVideoControl，用于控制拍照
    IAMVideoControl *pAMVidControl = NULL;
    hr = pDeviceFilter->QueryInterface(IID_IAMVideoControl, (void **)&pAMVidControl);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    // 9. 运行filter graph
    hr = pControl->Run();
    cout << dec << __LINE__ << '\t' << hex << hr << endl;

    IPin *pPin1 = NULL;
    hr = pBuild->FindPin(pDeviceFilter, PINDIR_OUTPUT, &PIN_CATEGORY_STILL, NULL, FALSE, 0, &pPin1);
    cout << dec << __LINE__ << '\t' << hex << hr << endl;
    // pAMVidControl->SetMode(pPin, VideoControlFlag_Trigger);
    // pPin->Release();

    int flag = 0;
    while (flag == 0)
    {
        pAMVidControl->SetMode(pPin1, VideoControlFlag_Trigger);
        pPin1->Release();

        // 拍照
        cin >> flag;
    }

    pControl->Stop();
}