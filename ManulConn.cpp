/**
 * 纯手工打造构建filter graph，实现视频预览和抓图
 * 
 * filter graph链路图：
 * video device(KsProxy) filter [OUT capture pin] --> [IN input pin] smart tee filter [OUT capture pin] --> [IN input pin] sample grabber filter [OUT output pin] --> [IN input pin] NULL filter
 *                                                                                    [OUT preview pin] --> [IN XForm input pin] MJPEG Decompressor filter [OUT XForm out pin] --> [IN input pin] VMR7 filter
 * 
 * 
 * 编译：cl ManulConn.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib
 * 
 * directshow自带智能连接，如果两个pin不能直接连接，中间会自动插入转换filter
*/
#include <afxwin.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
using namespace std;
#include "SaveGraphFile.h"

const BSTR szVideoDeviceName = L"SnIris Camera";

#pragma region(qedit.h中sample grabber 和 NULL filter相关的定义)
// sample grabber 和 NULL filter相关的定义
EXTERN_C const CLSID CLSID_SampleGrabber;

#ifdef __cplusplus

class DECLSPEC_UUID("C1F400A0-3F08-11d3-9F0B-006008039E37")
    SampleGrabber;
#endif

EXTERN_C const CLSID CLSID_NullRenderer;

#ifdef __cplusplus

class DECLSPEC_UUID("C1F400A4-3F08-11d3-9F0B-006008039E37")
    NullRenderer;
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

#pragma endregion

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

        sprintf(path, "pic/%d_%d.jpg", m_index, index++);
        FILE *file = fopen((const char *)path, "wb");
        fwrite(pBuffer, lBufferSize, 1, file);
        fclose(file);

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime, IMediaSample *pSample)
    {
        cout << "SampleCB SampleTime " << dec << SampleTime << endl;

        return S_OK;
    }
};

HRESULT SelectPin(IEnumPins *pEnumPins, PIN_DIRECTION pinDirection, LPWSTR szPinId, IPin **pSelected)
{
    HRESULT hr = S_FALSE;
    *pSelected = NULL;
    IPin *pPin = NULL;
    while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION pdir;
        pPin->QueryDirection(&pdir);
        // 查找输出pin
        if (pinDirection == pdir)
        {
            LPWSTR pid;
            pPin->QueryId(&pid);
            _tprintf(_T("device pin name %ws\r\n"), pid);

            if (szPinId == NULL || wcscmp(pid, szPinId))
            {
                *pSelected = pPin;
            }
        }
        pPin->Release();
        pPin = NULL;

        if (*pSelected != NULL)
        {
            hr = S_OK;
            break;
        }
    }

    if (hr == S_OK)
    {
        cout << "find success " << endl;
    }
    else
    {
        cout << "find failed " << endl;
    }

    pEnumPins->Reset();

    return hr;
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
            // ShowFilterInfo(info.pFilter);
            info.pFilter->Release();
        }
    }
    else
    {
        cout << "get pin info failed" << endl;
    }
}

void ShowFilterPinsInfo(IEnumPins *pEnumPins)
{
    IPin *pPin = NULL;
    while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    {
        ShowPinInfo(pPin);
        pPin->Release();
    }
    pEnumPins->Reset();
}

int main()
{
    HRESULT hr = CoInitialize(NULL);

    // 构建filter graph manager
    IGraphBuilder *pGraph = NULL;
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

    // 获取filter graph manager的控制接口和事件接口
    IMediaControl *pControl;
    IMediaEventEx *pEvent;
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEventEx, (void **)&pEvent);

    // 获取输入设备的filter
    IBaseFilter *pDevFilter = NULL;
    ICreateDevEnum *pSysDevEnum = NULL;
    IEnumMoniker *pEnumCat = NULL;
    IMoniker *pMoniker = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
    pSysDevEnum->Release();
    pSysDevEnum = NULL;
    bool bGetDevice = false;
    while (pEnumCat->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag *pPropBag = NULL;
        // 获取设备信息
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
        if (SUCCEEDED(hr))
        {
            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            if (SUCCEEDED(hr))
            {
                // Display the name in your UI somehow.
                _tprintf(_T("device name %ws\r\n"), varName.bstrVal);
            }

            if (wcscmp(szVideoDeviceName, varName.bstrVal) == 0)
            {
                bGetDevice = true;
                // 获取封装设备的Filter
                hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)&pDevFilter);
            }

            VariantClear(&varName);
            pPropBag->Release();
        }
        pMoniker->Release();
        pMoniker = NULL;

        if (bGetDevice)
            break;
    }
    pEnumCat->Release();
    pEnumCat = NULL;

    if (!bGetDevice)
    {
        cout << "find out device failed" << endl;
        return -1;
    }

    // 创建smart tee，由于输入设备没有preview输出pin，需要添加一个三通smart tee
    IBaseFilter *pSmartTeeFilter = NULL;
    hr = CoCreateInstance(CLSID_SmartTee, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pSmartTeeFilter);

    // 创建MJPEG Decompressor Filter，解码mjpg
    IBaseFilter *pMjpegDecFilter = NULL;
    hr = CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pMjpegDecFilter);

    // 使用VMR-7渲染视频，也可以使用VMR-9
    IBaseFilter *pVideoRenderFilter = NULL;
    // CLSID_VideoRendererDefault 在xp及以上操作系统默认使用VMR-7，xp以下采用旧版本的Video Render
    // hr = CoCreateInstance(CLSID_VideoRendererDefault, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pVideoRenderFilter);
    // 也可以直接指定VMR-7
    // hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pVideoRenderFilter);
    // 或者可以指定VMR-9
    // hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pVideoRenderFilter);
    hr = CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pVideoRenderFilter);

    // 窗口sample grabber用于抓图；由于sample grabber不在dshow.h中定义，而是在qedit.h中定义的；为了不引入其他乱七八糟的东西，我就把sample grabber相关的拷贝过来了，NULL Renderer Filter也一样
    IBaseFilter *pSampleGrabberFilter = NULL;
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pSampleGrabberFilter);
    // sample grabber需要一个接收sample输出帧的ISampleGrabberCB回调类
    ISampleGrabber *pSampleGrabber = NULL;
    hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void **)&pSampleGrabber);
    // 设置回调参数
    hr = pSampleGrabber->SetBufferSamples(TRUE);
    hr = pSampleGrabber->SetOneShot(TRUE);
    CSampleGrabberCB *pSampleGrabberCB = new CSampleGrabberCB;
    hr = pSampleGrabber->SetCallback(pSampleGrabberCB, 1);
    // 设置允许接收的视频类型(MJPG)
    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    //mt.subtype = MEDIASUBTYPE_YUY2;
    mt.subtype = MEDIASUBTYPE_MJPG;
    mt.formattype = FORMAT_VideoInfo;
    hr = pSampleGrabber->SetMediaType(&mt);
    pSampleGrabber->Release();
    pSampleGrabber = NULL;

    // sample grabber是有输出pin的，需要一个null renderer filter接收；null renderer filter用于丢弃不需要的输出
    IBaseFilter *pNullRendererFilter = NULL;
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pNullRendererFilter);

    // 所有的filter都准备好了，首先添加到filter graph中
    hr = pGraph->AddFilter(pDevFilter, L"video device");
    hr = pGraph->AddFilter(pSmartTeeFilter, L"smart tee");
    hr = pGraph->AddFilter(pSampleGrabberFilter, L"sample grabber");
    hr = pGraph->AddFilter(pNullRendererFilter, L"null renderer");
    hr = pGraph->AddFilter(pMjpegDecFilter, L"mjpg dec");
    hr = pGraph->AddFilter(pVideoRenderFilter, L"video renderer");

    // 再把这些filter都连接起来
    IEnumPins *pEnumPins = NULL;
    IPin *pPin = NULL;

    // 1. 找到设备的capture pin
    cout << "find device" << endl;
    IPin *pDevCapturePin = NULL;
    hr = pDevFilter->EnumPins(&pEnumPins);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_OUTPUT, L"Capture", &pDevCapturePin);
    // while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
    // {
    //     PIN_DIRECTION pdir;
    //     hr = pPin->QueryDirection(&pdir);
    //     // 查找输出pin
    //     if (PIN_DIRECTION::PINDIR_OUTPUT == pdir)
    //     {
    //         LPWSTR pid;
    //         hr = pPin->QueryId(&pid);
    //         _tprintf(_T("device pin name %ws\r\n"), pid);

    //         if (wcscmp(pid, L"Capture"))
    //         {
    //             pDevCapturePin = pPin;
    //         }
    //     }
    //     pPin->Release();
    //     pPin = NULL;

    //     if (pDevCapturePin != NULL)
    //         break;
    // }
    // pEnumPins->Release();
    // pEnumPins = NULL;

    if (pDevCapturePin == NULL)
    {
        cout << "not found device capture out pin" << endl;
        return -1;
    }

    // 查找smart tee的输入、preview输出、capture输出pin
    cout << "find smart tee" << endl;
    IPin *pStInPin = NULL;
    IPin *pStPrevPin = NULL;
    IPin *pStCapPin = NULL;
    hr = pSmartTeeFilter->EnumPins(&pEnumPins);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_INPUT, NULL, &pStInPin);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_OUTPUT, L"Capture", &pStCapPin);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_OUTPUT, L"Preview", &pStPrevPin);
    pEnumPins->Release();

    // 查找sample grabber的输入输出pin
    cout << "find sample grabber" << endl;
    IPin *pSgInPin = NULL;
    IPin *pSgOutPin = NULL;
    hr = pSampleGrabberFilter->EnumPins(&pEnumPins);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_INPUT, NULL, &pSgInPin);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_OUTPUT, NULL, &pSgOutPin);
    pEnumPins->Release();

    // 查找null renderer的输入pin
    cout << "find null renderer" << endl;
    IPin *pNullInPin = NULL;
    hr = pNullRendererFilter->EnumPins(&pEnumPins);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_INPUT, NULL, &pNullInPin);
    pEnumPins->Release();

    // 查找mjpeg decompressor的输入输出pin
    cout << "find mjpeg decompressor" << endl;
    IPin *pMdInPin = NULL;
    IPin *pMdOutPin = NULL;
    hr = pMjpegDecFilter->EnumPins(&pEnumPins);
    ShowFilterPinsInfo(pEnumPins);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_INPUT, NULL, &pMdInPin);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_OUTPUT, NULL, &pMdOutPin);
    pEnumPins->Release();

    // 查找video renderer的输入pin
    cout << "find video renderer" << endl;
    IPin *pVrInPin = NULL;
    hr = pVideoRenderFilter->EnumPins(&pEnumPins);
    SelectPin(pEnumPins, PIN_DIRECTION::PINDIR_INPUT, NULL, &pVrInPin);
    pEnumPins->Release();

    cout << __LINE__ << endl;

    // 连接所有的pin
    hr = pGraph->Connect(pDevCapturePin, pStInPin);
    cout << __LINE__ << " - " << hex << hr << " pin " << pDevCapturePin << " " << pStInPin << endl;
    hr = pGraph->Connect(pStCapPin, pSgInPin);
    cout << __LINE__ << " - " << hex << hr << endl;
    hr = pGraph->Connect(pStPrevPin, pMdInPin);
    cout << __LINE__ << " - " << hex << hr << endl;
    hr = pGraph->Connect(pSgOutPin, pNullInPin);
    cout << __LINE__ << " - " << hex << hr << endl;
    hr = pGraph->Connect(pMdOutPin, pVrInPin);
    cout << __LINE__ << " - " << hex << hr << endl;

    // 把graph打印出来
    SaveGraphFile(pGraph, L"mc.grf");

    // 动起来
    hr = pControl->Run();
    cout << "run " << hr << endl;
    Sleep(1000 * 100);

    CoUninitialize();
}