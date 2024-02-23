/**
 * 构建默认的usb设备视频预览
 * 
 * 编译
*/
#include "preview_wnd.h"

#include <afxwin.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
using namespace std;

#include <dvdmedia.h>
#include "SaveGraphFile.h"

IMediaControl *pControl = NULL;
IMediaEventEx *pEvent = NULL;

EXTERN_C const CLSID CLSID_SampleGrabber;

#ifdef __cplusplus

class DECLSPEC_UUID("C1F400A0-3F08-11d3-9F0B-006008039E37")
    SampleGrabber;
#endif

EXTERN_C const IID IID_ISampleGrabberCB;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
ISampleGrabberCB : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SampleCB(
        double SampleTime,
        IMediaSample *pSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE BufferCB(
        double SampleTime,
        BYTE *pBuffer,
        long BufferLen) = 0;
};

#else /* C style interface */

typedef struct ISampleGrabberCBVtbl
{
    BEGIN_INTERFACE

    HRESULT(STDMETHODCALLTYPE *QueryInterface)
    (
        ISampleGrabberCB *This,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)
    (
        ISampleGrabberCB *This);

    ULONG(STDMETHODCALLTYPE *Release)
    (
        ISampleGrabberCB *This);

    HRESULT(STDMETHODCALLTYPE *SampleCB)
    (
        ISampleGrabberCB *This,
        double SampleTime,
        IMediaSample *pSample);

    HRESULT(STDMETHODCALLTYPE *BufferCB)
    (
        ISampleGrabberCB *This,
        double SampleTime,
        BYTE *pBuffer,
        long BufferLen);

    END_INTERFACE
} ISampleGrabberCBVtbl;

interface ISampleGrabberCB
{
    CONST_VTBL struct ISampleGrabberCBVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define ISampleGrabberCB_QueryInterface(This, riid, ppvObject) \
    (This)->lpVtbl->QueryInterface(This, riid, ppvObject)

#define ISampleGrabberCB_AddRef(This) \
    (This)->lpVtbl->AddRef(This)

#define ISampleGrabberCB_Release(This) \
    (This)->lpVtbl->Release(This)

#define ISampleGrabberCB_SampleCB(This, SampleTime, pSample) \
    (This)->lpVtbl->SampleCB(This, SampleTime, pSample)

#define ISampleGrabberCB_BufferCB(This, SampleTime, pBuffer, BufferLen) \
    (This)->lpVtbl->BufferCB(This, SampleTime, pBuffer, BufferLen)

#endif /* COBJMACROS */

#endif /* C style interface */

#ifndef __ISampleGrabber_INTERFACE_DEFINED__
#define __ISampleGrabber_INTERFACE_DEFINED__

/* interface ISampleGrabber */
/* [unique][helpstring][local][uuid][object] */

EXTERN_C const IID IID_ISampleGrabber;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
ISampleGrabber : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(
        BOOL OneShot) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetMediaType(
        const AM_MEDIA_TYPE *pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(
        AM_MEDIA_TYPE * pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(
        BOOL BufferThem) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(
        /* [out][in] */ long *pBufferSize,
        /* [out] */ long *pBuffer) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(
        /* [retval][out] */ IMediaSample * *ppSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetCallback(
        ISampleGrabberCB * pCallback,
        long WhichMethodToCallback) = 0;
};

#else /* C style interface */

typedef struct ISampleGrabberVtbl
{
    BEGIN_INTERFACE

    HRESULT(STDMETHODCALLTYPE *QueryInterface)
    (
        ISampleGrabber *This,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)
    (
        ISampleGrabber *This);

    ULONG(STDMETHODCALLTYPE *Release)
    (
        ISampleGrabber *This);

    HRESULT(STDMETHODCALLTYPE *SetOneShot)
    (
        ISampleGrabber *This,
        BOOL OneShot);

    HRESULT(STDMETHODCALLTYPE *SetMediaType)
    (
        ISampleGrabber *This,
        const AM_MEDIA_TYPE *pType);

    HRESULT(STDMETHODCALLTYPE *GetConnectedMediaType)
    (
        ISampleGrabber *This,
        AM_MEDIA_TYPE *pType);

    HRESULT(STDMETHODCALLTYPE *SetBufferSamples)
    (
        ISampleGrabber *This,
        BOOL BufferThem);

    HRESULT(STDMETHODCALLTYPE *GetCurrentBuffer)
    (
        ISampleGrabber *This,
        /* [out][in] */ long *pBufferSize,
        /* [out] */ long *pBuffer);

    HRESULT(STDMETHODCALLTYPE *GetCurrentSample)
    (
        ISampleGrabber *This,
        /* [retval][out] */ IMediaSample **ppSample);

    HRESULT(STDMETHODCALLTYPE *SetCallback)
    (
        ISampleGrabber *This,
        ISampleGrabberCB *pCallback,
        long WhichMethodToCallback);

    END_INTERFACE
} ISampleGrabberVtbl;

interface ISampleGrabber
{
    CONST_VTBL struct ISampleGrabberVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define ISampleGrabber_QueryInterface(This, riid, ppvObject) \
    (This)->lpVtbl->QueryInterface(This, riid, ppvObject)

#define ISampleGrabber_AddRef(This) \
    (This)->lpVtbl->AddRef(This)

#define ISampleGrabber_Release(This) \
    (This)->lpVtbl->Release(This)

#define ISampleGrabber_SetOneShot(This, OneShot) \
    (This)->lpVtbl->SetOneShot(This, OneShot)

#define ISampleGrabber_SetMediaType(This, pType) \
    (This)->lpVtbl->SetMediaType(This, pType)

#define ISampleGrabber_GetConnectedMediaType(This, pType) \
    (This)->lpVtbl->GetConnectedMediaType(This, pType)

#define ISampleGrabber_SetBufferSamples(This, BufferThem) \
    (This)->lpVtbl->SetBufferSamples(This, BufferThem)

#define ISampleGrabber_GetCurrentBuffer(This, pBufferSize, pBuffer) \
    (This)->lpVtbl->GetCurrentBuffer(This, pBufferSize, pBuffer)

#define ISampleGrabber_GetCurrentSample(This, ppSample) \
    (This)->lpVtbl->GetCurrentSample(This, ppSample)

#define ISampleGrabber_SetCallback(This, pCallback, WhichMethodToCallback) \
    (This)->lpVtbl->SetCallback(This, pCallback, WhichMethodToCallback)

#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ISampleGrabber_SetOneShot_Proxy(
    ISampleGrabber *This,
    BOOL OneShot);

void __RPC_STUB ISampleGrabber_SetOneShot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ISampleGrabber_SetMediaType_Proxy(
    ISampleGrabber *This,
    const AM_MEDIA_TYPE *pType);

void __RPC_STUB ISampleGrabber_SetMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ISampleGrabber_GetConnectedMediaType_Proxy(
    ISampleGrabber *This,
    AM_MEDIA_TYPE *pType);

void __RPC_STUB ISampleGrabber_GetConnectedMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ISampleGrabber_SetBufferSamples_Proxy(
    ISampleGrabber *This,
    BOOL BufferThem);

void __RPC_STUB ISampleGrabber_SetBufferSamples_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ISampleGrabber_GetCurrentBuffer_Proxy(
    ISampleGrabber *This,
    /* [out][in] */ long *pBufferSize,
    /* [out] */ long *pBuffer);

void __RPC_STUB ISampleGrabber_GetCurrentBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ISampleGrabber_GetCurrentSample_Proxy(
    ISampleGrabber *This,
    /* [retval][out] */ IMediaSample **ppSample);

void __RPC_STUB ISampleGrabber_GetCurrentSample_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

HRESULT STDMETHODCALLTYPE ISampleGrabber_SetCallback_Proxy(
    ISampleGrabber *This,
    ISampleGrabberCB *pCallback,
    long WhichMethodToCallback);

void __RPC_STUB ISampleGrabber_SetCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);

#endif /* __ISampleGrabber_INTERFACE_DEFINED__ */

EXTERN_C const CLSID CLSID_NullRenderer;

#ifdef __cplusplus

class DECLSPEC_UUID("C1F400A4-3F08-11d3-9F0B-006008039E37")
    NullRenderer;
#endif

// class CDeviceController : public CFrameWnd
// {

// };

class CSampleGrabberCB : public ISampleGrabberCB
{
public:
    STDMETHODIMP_(ULONG)
    AddRef() { return 2; }
    STDMETHODIMP_(ULONG)
    Release() { return 1; }
    STDMETHODIMP QueryInterface(REFIID iid, void **ppv)
    {
        if (NULL == ppv)
            return E_POINTER;
        *ppv = NULL;
        if (IID_IUnknown == iid)
        {
            *ppv = (IUnknown *)this;
            AddRef();
            return S_OK;
        }
        else if (IID_ISampleGrabberCB == iid)
        {
            *ppv = (ISampleGrabberCB *)this;
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP BufferCB(double dblSampleTime, BYTE *pBuffer, long lBufferSize)
    {
        cout << "BufferCB SampleTime " << dec << dblSampleTime << ", BufferSize " << lBufferSize << endl;

        static char path[100];
        static int index = 0;
        static int m_index = 0;

        // sprintf(path, "pic/%d_%d.jpg", m_index, index++);
        // FILE *file = fopen((const char *)path, "wb");
        // fwrite(pBuffer, lBufferSize, 1, file);
        // fclose(file);

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime, IMediaSample *pSample)
    {
        cout << "SampleCB SampleTime " << dec << SampleTime << endl;

        return S_OK;
    }
};

void ShowGUID(GUID *guid)
{
    cout << hex << guid->Data1 << "-" << guid->Data2 << "-" << guid->Data3 << "-";
    for (int i = 0; i < 8; i++)
    {
        cout << hex << guid->Data4[i];
    }
    cout << endl;
}

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

void ShowPinInfo(IPin *pin)
{
    PIN_INFO info;
    HRESULT hr = pin->QueryPinInfo(&info);
    if (S_OK == hr)
    {
        _tprintf(_T("pin name %ws,direction %d\r\n"), info.achName, info.dir);
        if (NULL != info.pFilter)
        {
            ShowFilterInfo(info.pFilter);
            info.pFilter->Release();
        }
    }
    else
    {
        cout << "get pin info failed" << endl;
    }
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

int show(HWND hwnd)
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

    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

    ICaptureGraphBuilder2 *pBuild = NULL;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&pBuild);
    if (FAILED(hr))
        return 0;

    pBuild->SetFiltergraph(pGraph);

    // build graph
    IBaseFilter *pDeviceFilter = NULL;

    ChooseDevice(_T("SEMIC Camera"), &pDeviceFilter);

    cout << "Create Device Filter " << hex << pDeviceFilter << endl;
    pGraph->AddFilter(pDeviceFilter, L"SEMIC Camera");

    ShowFilterInfo(pDeviceFilter);

    IEnumPins *pDeviceEnumPins = NULL;
    // 设置输出视频格式及参数
    hr = pDeviceFilter->EnumPins(&pDeviceEnumPins);
    IPin *pDeviceOutPin = NULL;
    while (pDeviceEnumPins->Next(1, &pDeviceOutPin, NULL) == S_OK)
    {
        PIN_DIRECTION pdir;
        pDeviceOutPin->QueryDirection(&pdir);
        if (PIN_DIRECTION::PINDIR_OUTPUT == pdir)
        {
            break;
        }
        pDeviceOutPin->Release();
    }

    //
    GUID pSelectFormat = FORMAT_VideoInfo;
    GUID pSelectVideoType = MEDIASUBTYPE_MJPG;
    LONG pSelectWidth = 2592L;
    LONG pSelectHeight = 1544L;
    int pSelectFrameRate = 15;

    AM_MEDIA_TYPE *pSelectMediaType = NULL;

    AM_MEDIA_TYPE *pMediaType = NULL;

    IAMStreamConfig *pStreamConfig = NULL;
    hr = pDeviceOutPin->QueryInterface(IID_IAMStreamConfig, (void **)&pStreamConfig);
    if (hr == S_OK)
    {

        int piCount, piSize;
        hr = pStreamConfig->GetNumberOfCapabilities(&piCount, &piSize);
        cout << "show media type : " << endl;
        for (int i = 0; i < piCount; i++)
        {
            VIDEO_STREAM_CONFIG_CAPS scc;
            pStreamConfig->GetStreamCaps(i, &pMediaType, reinterpret_cast<BYTE *>(&scc)); // 获取所有设备支持的多媒体格式

            // hr = pStreamConfig->GetFormat(&pMediaType);
            // pMediaType->formattype;

            // ShowGUID(&pMediaType->formattype);
            // ShowGUID(&pMediaType->majortype);
            // ShowGUID(&pMediaType->subtype);
            // ShowGUID(&pMediaType->subtype);
            // pStreamConfig->SetFormat()
            if (FORMAT_VideoInfo == pMediaType->formattype)
            {
                VIDEOINFOHEADER *pvideoInfo = (VIDEOINFOHEADER *)pMediaType->pbFormat;
                cout << "format videoinfo frame type ";
                if (MEDIASUBTYPE_YUY2 == pMediaType->subtype)
                {
                    cout << "YUY2 , ";
                }
                else if (MEDIASUBTYPE_MJPG == pMediaType->subtype)
                {
                    cout << "MJPG , ";
                }
                else
                {
                    ShowGUID(&pMediaType->subtype);
                }

                cout << " height " << dec << pvideoInfo->bmiHeader.biHeight << " width " << pvideoInfo->bmiHeader.biWidth; // << endl;

                cout << ", frame rate " << dec << 10000000 / pvideoInfo->AvgTimePerFrame << endl;

                if (pSelectFormat == pMediaType->formattype && pSelectVideoType == pMediaType->subtype &&
                    pSelectHeight == pvideoInfo->bmiHeader.biHeight && pSelectWidth == pvideoInfo->bmiHeader.biWidth)
                {
                    pSelectMediaType = pMediaType;
                }
                // cout << " frame size " << dec << pvideoInfo->bmiHeader.biSizeImage << endl;

                // 修改帧率
                // int _frame_rate = 5;
                // pvideoInfo->AvgTimePerFrame = 10000000 / _frame_rate;
                // pvideoInfo->dwBitRate = pvideoInfo->bmiHeader.biSizeImage * 8 * _frame_rate;
                // pStreamConfig->SetFormat(pMediaType);
            }
            else if (FORMAT_VideoInfo2 == pMediaType->formattype)
            {
                VIDEOINFOHEADER2 *pvideoInfo = (VIDEOINFOHEADER2 *)pMediaType->pbFormat;
                cout << "format videoinfo2 frame type ";
                if (MEDIASUBTYPE_YUY2 == pMediaType->subtype)
                {
                    cout << "YUY2 , ";
                }
                else if (MEDIASUBTYPE_MJPG == pMediaType->subtype)
                {
                    cout << "MJPG , ";
                }
                else
                {
                    ShowGUID(&pMediaType->subtype);
                }

                cout << " height " << dec << pvideoInfo->bmiHeader.biHeight << " width " << pvideoInfo->bmiHeader.biWidth; // << endl;
                cout << ", frame rate " << dec << 10000000 / pvideoInfo->AvgTimePerFrame << endl;

                if (pSelectFormat == pMediaType->formattype && pSelectVideoType == pMediaType->subtype &&
                    pSelectHeight == pvideoInfo->bmiHeader.biHeight && pSelectWidth == pvideoInfo->bmiHeader.biWidth)
                {
                    pSelectMediaType = pMediaType;
                }
            }
            else
            {
                cout << " other video format type ";
                ShowGUID(&pMediaType->formattype);
            }
        }
    }

    hr = pStreamConfig->SetFormat(pSelectMediaType);
    cout << " Set StreamConfig Media Format " << dec << hr << endl;
    pStreamConfig->Release();

    // 添加smart tee ， 分成两路，一路预览，一路抓图
    IBaseFilter *pSmartTee = NULL;
    // hr = CoCreateInstance(CLSID_SmartTee, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pSmartTee);
    // cout << " Create Smart Tee " << hex << hr << endl;
    // pGraph->AddFilter(pDeviceFilter, L"Smart Tee");

    IBaseFilter *pNullFilter = NULL;
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&pNullFilter);

    pGraph->AddFilter(pNullFilter, L"NULL Filter");

    // RenderStream 若filter没有preview out pin，只有capture out pin则会插入一个smart tee filter；若没有指定要连接的目标则默认为video/video render filter
    hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pDeviceFilter, NULL, NULL);
    // hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pDeviceFilter, NULL, pSmartTee);
    cout << " RenderStream 1 " << hex << hr << endl;

    // hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pSmartTee, NULL, NULL);
    // cout << " RenderStream 2 " << hex << hr << endl;
    // hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSmartTee, NULL, pNullFilter);
    // cout << " RenderStream 3 " << hex << hr << endl;

    // 查找插入的smart tee filter
    hr = pBuild->FindInterface(&LOOK_DOWNSTREAM_ONLY, &MEDIATYPE_Video, pDeviceFilter, IID_IBaseFilter, (void **)&pSmartTee);
    cout << " FindInterface 1 " << hex << hr << " Smart Tee : " << pSmartTee << endl;

    ShowFilterInfo(pSmartTee);

    //  hr = pBuild->FindInterface(NULL, &MEDIATYPE_Video, pDeviceFilter, IID_IBaseFilter, (void **)&pSmartTee);
    // cout << " FindInterface 1 " << hex << hr << " Smart Tee : " << pSmartTee << endl;

    // CLSID_MjpegDec;

    // IBaseFilter *pRender = NULL;
    // hr = pBuild->FindInterface(NULL, &MEDIATYPE_Video, pSmartTee, IID_IBaseFilter, (void **)&pRender);
    CLSID slcid;
    hr = pSmartTee->GetClassID(&slcid);
    if (hr == S_OK)
        cout << hex << slcid.Data1 << "-" << slcid.Data2 << "-" << slcid.Data3 << endl;
    else
    {
        cout << "get class id failed" << endl;
    }

    IPin *pPrev = NULL;
    IEnumPins *pEnumPins = NULL;
    hr = pSmartTee->EnumPins(&pEnumPins);
    while (pEnumPins->Next(1, &pPrev, NULL) == S_OK)
    {
        PIN_DIRECTION pDir;
        pPrev->QueryDirection(&pDir);

        if (pDir == PINDIR_OUTPUT)
        {
            LPWSTR pId;
            pPrev->QueryId(&pId);
            // cout << pId << endl;

            _tprintf(_T("pin id : %ws , direction %d\r\n"), pId, pDir);

            // PIN_INFO pPinInfo;
            // pPrev->QueryPinInfo(&pPinInfo);

            pPrev->Release();
            // ShowPinInfo(pPrev);
        }

        /* code */
    }

    pEnumPins->Release();

    // IPin * pNextPin = NULL;
    // IBaseFilter * pNextFilter = NULL;
    // ULONG pins=1;
    // pPrev->QueryInternalConnections(&pNextPin,&pins);
    // cout <<"start show next pin "<<endl;
    // ShowPinInfo(pPrev);
    // ShowPinInfo(pNextPin);

    // pPrev->Release();

    // pBuild->

    // FILTER_INFO finfo;
    // pSmartTee->QueryFilterInfo(&finfo);
    // cout << finfo.achName << endl;

    // grabber sample
    IBaseFilter *pGrabberF = NULL;
    ISampleGrabber *pGrabber = NULL;
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pGrabberF);

    cout << " Create Sample Grabber " << hex << hr << endl;

    hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");

    cout << " add grabber filter " << hex << hr << endl;

    hr = pGrabberF->QueryInterface(IID_ISampleGrabber, (void **)&pGrabber);

    cout << "grabber filter query interface " << hex << hr << endl;

    // 设置允许的视频格式
    // AM_MEDIA_TYPE mt;
    // ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    // mt.majortype = MEDIATYPE_Video;
    // mt.subtype = MEDIASUBTYPE_MJPG;
    // mt.formattype = FORMAT_VideoInfo;
    // hr = pGrabber->SetMediaType(&mt); // 在使用前必须设置多媒体类型
    hr = pGrabber->SetMediaType(pSelectMediaType);
    cout << " Set Media Type " << dec << hr << endl;

    // RenderStream 会查找符合条件的未连接的PIN进行连接
    hr = pBuild->RenderStream(NULL, &MEDIATYPE_Video, pSmartTee, pGrabberF, pNullFilter);
    cout << " RenderStream 4 " << hex << hr << endl;
    // hr = pBuild->RenderStream(NULL, &MEDIATYPE_Video, pGrabberF, NULL, pNullFilter);
    // cout << " RenderStream 3 " << hex << hr << endl;

    // 保存filer graph文件，用graphedit打开
    SaveGraphFile(pGraph, L"MyGraph.grf");

    CSampleGrabberCB *scb = new CSampleGrabberCB;
    pGrabber->SetCallback(scb, 1);

    // hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pDeviceFilter, NULL, NULL);
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

    // 设置视频播放窗口，如果不设置视频播放窗口，则会单独开一个窗口播放
    IVideoWindow *pVidWin = NULL;
    pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVidWin);
    pVidWin->put_Owner((OAHWND)hwnd);
    pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    RECT rc;
    GetClientRect(hwnd, &rc);
    pVidWin->SetWindowPosition(0, 0, rc.right, rc.bottom);

    // 设置filter graph event
    // pEvent->SetNotifyWindow((OAHWND)g_wnd_hwnd, WM_FGNOTIFY, 0);
    pEvent->SetNotifyFlags(AM_MEDIAEVENT_NONOTIFY);

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