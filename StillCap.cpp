//无窗模式，可按钮触发截图
#include <Windows.h> 
#include <tchar.h> 
#include <DShow.h> 
#include <atlbase.h> 
#include <streams.h> 
#include <assert.h> 
 
#include <d3d9.h>
#include <vmr9.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include <qedit.h>
 
#define DEFAULT_VIDEO_WIDTH     640 
#define DEFAULT_VIDEO_HEIGHT    480 
#define WIN_STYLE (WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX) 
#define EXT_STYLE (WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW) 
 
enum PLAYSTATE {Stopped, Paused, Running, Init}; 
 
typedef HANDLE zbar_event_t; 
typedef struct zbar_mutex_s { 
 int count; 
 CRITICAL_SECTION mutex; 
} zbar_mutex_t; 
 
struct video_state_s { 
 HANDLE captured; 
 HWND hwnd;                  /* vfw interface */
 HANDLE notify;              /* capture thread status change */
 int bi_size;                /* size of bih */
 BITMAPINFOHEADER *bih;      /* video format details */
 
 
 //主要的用于摄像头预览
 IGraphBuilder* m_pGraph;
 ICaptureGraphBuilder2* m_pBuilder;
 IBaseFilter* pSrcFilter;
 IMediaControl* g_pMC;
 IVideoWindow* g_pVW;
 IMediaEventEx * g_pME; 
 CComPtr<ISampleGrabber> pGrabber; 
 HINSTANCE hInstance; 
 
 //静态拍照
 IAMVideoControl* m_pAMVideoControl;
 IBaseFilter* m_pTSG_Filter;
 ISampleGrabber* m_pTSG;
 IBaseFilter *m_pNull;
 
 AM_MEDIA_TYPE mt; 
 AM_MEDIA_TYPE g_StillMediaType; 
 
 
 IVMRWindowlessControl9 *m_pVMR9;
 IBaseFilter *m_pRenderFilter;
 IVMRFilterConfig* m_pConfig;;
}; 
 
 
struct zbar_processor_s { 
 const void *userdata;               /* application data */
 unsigned req_width, req_height;     /* application requested video size */
 int req_intf, req_iomode;           /* application requested interface */
 int input;                          /* user input status */
 int threaded; 
 int visible;                        /* output window mapped to display */
 int streaming;                      /* video enabled */
 int dumping;                        /* debug image dump */
 void *display;                      /* X display connection */
 unsigned long xwin;                 /* toplevel window */
 zbar_mutex_t mutex;                 /* shared data mutex */
 int lock_level; 
 HINSTANCE hInstance; 
}; 
 
 
typedef struct video_state_s video_state_t; 
 
 
static inline int _zbar_mutex_init (zbar_mutex_t *lock) 
{ 
 lock->count = 1; 
 InitializeCriticalSection(&lock->mutex); //函数功能初始化一个临界资源对象 
 return(0); 
} 
 
static inline void _zbar_mutex_destroy (zbar_mutex_t *lock) 
{ 
 DeleteCriticalSection(&lock->mutex); 
} 
 
static inline int _zbar_mutex_lock (zbar_mutex_t *lock) 
{ 
 EnterCriticalSection(&lock->mutex); 
 if(lock->count++ < 1) 
  assert(0); 
 return(0); 
} 
 
static inline int _zbar_mutex_unlock (zbar_mutex_t *lock) 
{ 
 if(lock->count-- <= 1) 
  assert(0); 
 LeaveCriticalSection(&lock->mutex); 
 return(0); 
} 
class CSampleGrabberCB : public ISampleGrabberCB
{
public:
 long Width;
 long Height;
 HWND cb_hwnd;
 bool bFirst;
 
 CSampleGrabberCB( )
 {
  Width = 0;
  Height = 0;
  cb_hwnd = NULL;
  bFirst = false;
 } 
 
 
 STDMETHODIMP_(ULONG) AddRef() { return 1; }
 STDMETHODIMP_(ULONG) Release() { return 2; }
 STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
 {
  if (NULL == ppvObject) 
   return E_POINTER;
  if (riid == __uuidof(IUnknown))
  {
   *ppvObject = static_cast<IUnknown*>(this);
   return S_OK;
  }
  if (riid == __uuidof(ISampleGrabberCB))
  {
   *ppvObject = static_cast<ISampleGrabberCB*>(this);
   return S_OK;
  }
  return E_NOTIMPL;
 }
 
 STDMETHODIMP SampleCB(double SampleTime,IMediaSample* pSample)
 {
  return E_NOTIMPL;
 }
 STDMETHODIMP BufferCB(double Time,BYTE* pBuffer,long lBufferSize)
 {
  video_state_t *state = (video_state_t *)GetWindowLong(cb_hwnd,0);
 
  HWND cb_hwndTemp = (HWND)GetWindowLong(cb_hwnd,4);
 
  if (!bFirst)//初次启动会有消息过来
  {
   bFirst = true;
   return S_OK;
  }
  if (!pBuffer)
   return E_POINTER;
 
  if ((state->g_StillMediaType.majortype != MEDIATYPE_Video) ||
   (state->g_StillMediaType.formattype != FORMAT_VideoInfo) ||
   (state->g_StillMediaType.cbFormat < sizeof(VIDEOINFOHEADER)) ||
   (state->g_StillMediaType.pbFormat == NULL))
   return VFW_E_INVALIDMEDIATYPE;
 
  HANDLE hf = CreateFile("D:\\Example.bmp",GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
  if (hf == INVALID_HANDLE_VALUE)
   return FALSE;
 
  long cbBitmapInfoSize = state->g_StillMediaType.cbFormat-SIZE_PREHEADER;
  VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)state->g_StillMediaType.pbFormat;
  BITMAPFILEHEADER bfh;
  ZeroMemory(&bfh,sizeof(bfh));
  bfh.bfType = 'MB'; // Little-endian for "MB".
  bfh.bfSize = sizeof(bfh)+lBufferSize+cbBitmapInfoSize;
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+cbBitmapInfoSize;
 
  // Write the file header.
  DWORD dwWritten = 0;
  WriteFile(hf,&bfh,sizeof(bfh),&dwWritten,NULL);
  WriteFile(hf,HEADER(pVideoHeader),cbBitmapInfoSize,&dwWritten,NULL); 
  WriteFile(hf,pBuffer,lBufferSize,&dwWritten,NULL);
  CloseHandle(hf);
 
  return S_OK;
 }
 
};
 
void ResizeVideoWindow(video_state_t *state) 
{ 
 // Resize the video preview window to match owner window size 
 if (state->g_pVW) 
 { 
  RECT rc; 
 
  GetClientRect(state->hwnd, &rc); 
 
  state->g_pVW->SetWindowPosition(0,0,rc.right,rc.bottom); 
 
 } 
} 
 
HRESULT SetupVideoWindow(video_state_t *state) 
{ 
 HRESULT hr; 
 
 // Set the video window to be a child of the main window 
 hr =state->g_pVW->put_Owner((OAHWND)state->hwnd); 
 if (FAILED(hr)) 
 {
  MessageBox(NULL, _T("error_19!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return hr; 
 }
 
 // Set video window style 
 hr = state->g_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN); 
 if (FAILED(hr)) 
 {
  MessageBox(NULL, _T("error_20!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return hr; 
 }
 
 ResizeVideoWindow(state); 
 
 hr = state->g_pVW->put_Visible(OATRUE); 
 if (FAILED(hr)) 
 {
  MessageBox(NULL, _T("error_21!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return hr; 
 }
 return hr; 
} 
 
int HandleGraphEvent(video_state_t *state ) 
{ 
 LONG evCode, evParam1, evParam2; 
 HRESULT hr=S_OK; 
 
 if(state) 
 { 
  if (!state->g_pME) 
   return E_POINTER; 
 
  while(SUCCEEDED(state->g_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,  
   (LONG_PTR *) &evParam2, 0))) 
  { 
   hr = state->g_pME->FreeEventParams(evCode, evParam1, evParam2); 
  } 
 } 
 
 return 0; 
} 
 
bool changeVideoWindowPos(video_state_t *state ,long inLeft, long inTop, long inWidth, long inHeight) 
{ 
 //video_state_t *state = video->state; 
 if (state->g_pVW) 
 { 
  long lVisible = OATRUE; 
  state->g_pVW->get_Visible(&lVisible); 
  // Hide the video window first 
  state->g_pVW->put_Visible(OAFALSE);    
  state->g_pVW->put_Left(inLeft); 
  state->g_pVW->put_Top(inTop); 
  state->g_pVW->put_Width(inWidth); 
  state->g_pVW->put_Height(inHeight); 
 
  state->g_pVW->put_Visible(lVisible); 
  return true; 
 } 
 return false; 
} 
 
int OwnerMessage(HWND hwnd,UINT message, 
     WPARAM wparam, 
     LPARAM lparam) 
{ 
 video_state_t *state=(video_state_t *)GetWindowLong(hwnd,GWL_USERDATA); 
 
 return 0; 
} 
 
 
 
HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter) 
{ 
 HRESULT hr; 
 IBaseFilter * pSrc = NULL; 
 CComPtr <IMoniker> pMoniker =NULL; 
 ULONG cFetched; 
 
 if (!ppSrcFilter) 
  return E_POINTER; 
 
 CComPtr <ICreateDevEnum> pDevEnum =NULL; 
 
 hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, 
  IID_ICreateDevEnum, (void **) &pDevEnum); 
 if (FAILED(hr)) 
 { 
  return hr; 
 } 
 
 CComPtr <IEnumMoniker> pClassEnum = NULL; 
 
 hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0); 
 if (FAILED(hr)) 
 { 
  return hr; 
 } 
 
 if (pClassEnum == NULL) 
 { 
  return E_FAIL; 
 } 
 
 if (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched))) 
 { 
  hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc); 
  if (FAILED(hr)) 
  { 
   return hr; 
  } 
 } 
 else
 { 
  return E_FAIL; 
 } 
 
 *ppSrcFilter = pSrc; 
 
 return hr; 
}
 
int GetUSBCameraDeviceID()
{
 int id = -1;
 
 ICreateDevEnum *pCreateDevEnum;
 HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);
 if (hr != NOERROR)
 {
  return id;
 }
 
 CComPtr<IEnumMoniker> pEm;
 hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
 if (hr != NOERROR)
 {
  return id;
 }
 
 pEm->Reset();
 ULONG cFetched;
 IMoniker *pM;
 bool bBreak = false;
 while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
 {
  IPropertyBag *pBag;
  hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
  if(SUCCEEDED(hr)) 
  {
   VARIANT var;
   var.vt = VT_BSTR;
   hr = pBag->Read(L"FriendlyName", &var, NULL);
   if (hr == NOERROR) 
   {
    id++;
    char str[2048]; 
    WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
    SysFreeString(var.bstrVal);
 
    if (memcmp(str,"USB 视频设备",8)==0)
     bBreak = true;
 
   }
   pBag->Release();
  }
  pM->Release();
  if (bBreak)
   break;
 }
 
 return id;
}
 
 
BOOL BindFilter(int deviceId,IBaseFilter **pFilter)
{
 if (deviceId < 0) 
  return false;
 
 CComPtr<ICreateDevEnum> pCreateDevEnum;
 HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**)&pCreateDevEnum);
 if (hr != NOERROR)
 {
  return false;
 }
 CComPtr<IEnumMoniker> pEm;
 hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
 if (hr != NOERROR)
 {
  return false;
 }
 
 pEm->Reset();
 ULONG cFetched;
 IMoniker *pM;
 int index = 0;
 while(hr = pEm->Next(1, &pM, &cFetched),hr==S_OK,index <= deviceId)
 {
  IPropertyBag *pBag;
  hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
  if(SUCCEEDED(hr)) 
  {
   VARIANT var;
   var.vt = VT_BSTR;
   hr = pBag->Read(L"FriendlyName", &var, NULL);
   if (hr == NOERROR) 
   {
    if (index == deviceId)
     pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
    SysFreeString(var.bstrVal);
   }   
   pBag->Release();
  }
  pM->Release();
  index++;
 }
 
 return true;
}
 
 
HRESULT GetInterfaces(video_state_t *state) 
{ 
 HRESULT hr; 
 
 hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **) &state->m_pGraph); 
 if (FAILED(hr)) 
  return hr; 
 
 hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **) &state->m_pBuilder); 
 if (FAILED(hr)) 
  return hr; 
 
 state->m_pBuilder->SetFiltergraph(state->m_pGraph);
 
 hr = state->m_pGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &state->g_pMC); 
 if (FAILED(hr)) 
  return hr; 
 
 hr = state->m_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) &state->g_pVW); 
 if (FAILED(hr)) 
  return hr; 
 
 hr = state->m_pGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) &state->g_pME); 
 if (FAILED(hr)) 
  return hr; 
 
 hr = CoCreateInstance(CLSID_SampleGrabber,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&state->m_pTSG_Filter);
 if(FAILED(hr))
  return hr;
 
 hr = state->m_pTSG_Filter->QueryInterface(IID_ISampleGrabber,(void**)&state->m_pTSG);
 if(FAILED(hr))
  return hr;
 
 
 hr = CoCreateInstance(CLSID_NullRenderer,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&state->m_pNull);
 if(FAILED(hr))
  return hr;
 
 
 hr = state->g_pME->SetNotifyWindow((OAHWND)state->hwnd, WM_APP+1, 0); 
 
 return hr; 
} 
 
 
 
static LRESULT CALLBACK win_handle_event (HWND hwnd, 
            UINT message, 
            WPARAM wparam, 
            LPARAM lparam) 
{ 
 video_state_t *state=(video_state_t *)GetWindowLong(hwnd,GWL_USERDATA); 
 switch(message) { 
 
case WM_APP+1: {    //32769 
 HandleGraphEvent(state); 
 break; 
      } 
 
case WM_CLOSE: { 
 
 exit(0); 
      } 
 
case WM_PAINT: { 
 PAINTSTRUCT ps; 
 BeginPaint(hwnd, &ps); 
 EndPaint(hwnd, &ps); 
 return(0); 
      } 
 
 } 
 
 return(DefWindowProc(hwnd, message, wparam, lparam)); 
} 
 
 
 
static inline ATOM win_register_class (HINSTANCE hmod) 
{ 
 BYTE and_mask[1] = { 0xff }; 
 BYTE xor_mask[1] = { 0x00 }; 
 
 WNDCLASSEX wc = { sizeof(WNDCLASSEX), 0, }; 
 wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
 wc.hInstance = hmod; 
 wc.lpfnWndProc = win_handle_event; 
 wc.lpszClassName = "_LaunchBar Class"; 
 wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  
 wc.hbrBackground =  (HBRUSH) (COLOR_WINDOW + 1); 
 wc.hIcon         = LoadIcon( hmod, IDI_APPLICATION ); 
 wc.hCursor         = LoadCursor( NULL, IDC_ARROW ); 
 wc.cbWndExtra     =8; 
 
 return(RegisterClassEx(&wc)); 
} 
 
 
 
int _zbar_event_init (zbar_event_t *event) 
{ 
 *event = CreateEvent(NULL, 0, 0, NULL); 
 return((*event) ? 0 : -1); 
} 
 
 
HRESULT InitWindowlessVMR(video_state_t *state)
{
 
 if (!state->m_pGraph ) return E_POINTER;
 
 HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL,
  CLSCTX_INPROC, IID_IBaseFilter, (void**)&state->m_pRenderFilter);
 
 if (FAILED(hr))
 {
  return hr;
 }
 
 // Add the VMR to the filter graph.
 hr = state->m_pGraph->AddFilter(state->m_pRenderFilter, L"Video Mixing Renderer");
 if (FAILED(hr))
 {
  state->m_pRenderFilter->Release();
  return hr;
 }
 
 // Set the rendering mode.
 hr = state->m_pRenderFilter->QueryInterface(IID_IVMRFilterConfig9, (void**)&state->m_pConfig);
 if (SUCCEEDED(hr))
 {
  hr = state->m_pConfig->SetRenderingMode(VMRMode_Windowless);
 }
 
 if (SUCCEEDED(hr))
 {
  hr = state->m_pRenderFilter->QueryInterface(IID_IVMRWindowlessControl9, (void**)&state->m_pVMR9);
 
  if( SUCCEEDED(hr))
  {
   // 获取传入窗口的区域，以设置显示窗口
   RECT rcDestR,rcDest;
   ::GetClientRect (state->hwnd,&rcDest);
 
   rcDestR.left =60;
   rcDestR.right=rcDest.right-60;
   rcDestR.top =40;
   rcDestR.bottom=rcDest.bottom-80;
 
   hr = state->m_pVMR9->SetVideoPosition(NULL, &rcDestR);
   hr = state->m_pVMR9->SetVideoClippingWindow(state->hwnd);
 
   if (FAILED(hr))
   {
    // An error occurred, so release the interface.
    state->m_pVMR9->Release();
   }
  }
 }
 
 //pVmr->Release();
 return hr;
}
 
 
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hInstP, LPSTR lpCmdLine, int nCmdShow) 
{ 
 if(FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) 
 { 
  exit(1); 
 }  
 
 zbar_processor_s *proc = (zbar_processor_s *)calloc(1, sizeof(zbar_processor_s)); 
 if(!proc) 
  return(NULL); 
 
 MSG msg={0}; 
 
 ATOM wca = win_register_class(hInstance); 
 
 if(!wca) 
  return 0;  
 
 RECT r = { 0, 0, DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT }; 
 
 AdjustWindowRectEx(&r, WIN_STYLE, 0, EXT_STYLE); 
 
 video_state_t *state = (video_state_t *)calloc(1, sizeof(video_state_t)); 
 
 proc->hInstance = hInstance; 
 
 state->hInstance = hInstance; 
 
 state->hwnd = CreateWindowEx(EXT_STYLE, (LPCTSTR)(long)wca, 
  "LaunchBar", WIN_STYLE, 
  CW_USEDEFAULT, CW_USEDEFAULT, 
  1024, 768, 
  NULL, NULL, hInstance, proc); 
 
 //  int FDeviceID = GetUSBCameraDeviceID();
 //  if (FDeviceID == -1)
 //   return(-1);
 
 
 LONG ret1 = SetWindowLong(state->hwnd,0,LONG(state)); 
 ret1 = SetWindowLong(state->hwnd,4,LONG(state->hwnd)); 
 
 HRESULT ret = GetInterfaces(state); 
 if (FAILED(ret)) 
 {
  MessageBox(NULL, _T("error_1!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1); 
 }
 
 //  if(!BindFilter(FDeviceID, &state->pSrcFilter))
 //   return(-1); 
 
 ret = FindCaptureDevice(&state->pSrcFilter);//选择一个视频捕捉设备 
 if (FAILED(ret)) 
 {
  MessageBox(NULL, _T("error_2!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1); 
 }
 
 ret = state->m_pGraph->AddFilter(state->pSrcFilter, L"Capture Filter"); 
 if (FAILED(ret)) 
 {
  MessageBox(NULL, _T("error_3!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1); 
 }
 
 ret = state->m_pGraph->AddFilter( state->m_pTSG_Filter, L"Grabber" ); 
 if (FAILED(ret)) 
 {
  MessageBox(NULL, _T("error_4!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1); 
 }
 
 ret = state->m_pGraph->AddFilter(state->m_pNull, L"NullRender"); 
 if (FAILED(ret)) 
 {
  MessageBox(NULL, _T("error_5!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1); 
 }
 
 InitWindowlessVMR(state);
 
 
 //设置视频格式
 ZeroMemory(&state->mt,sizeof(AM_MEDIA_TYPE));
 state->mt.majortype = MEDIATYPE_Video;
 state->mt.subtype = MEDIASUBTYPE_RGB24;
 ret = state->m_pTSG->SetMediaType(&state->mt);
 if (FAILED(ret)) 
 {
  MessageBox(NULL, _T("error_6!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1);
 }
 
 
 ret = state->m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,state->pSrcFilter,NULL, state->m_pRenderFilter);
 
 if(FAILED(ret))
 {
  MessageBox(NULL, _T("error_7!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  ret = state->m_pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,state->pSrcFilter,NULL, state->m_pRenderFilter);
 }
 if (ret != S_OK && ret != VFW_S_NOPREVIEWPIN)
 {
  MessageBox(NULL, _T("error_8!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1);
 }
 
 
 IAMVideoControl *pAMVidControl = NULL;
 ret = state->pSrcFilter->QueryInterface(IID_IAMVideoControl, (void**)&pAMVidControl);
 if (SUCCEEDED(ret))
 {
  // Find the still pin.
  IPin *pPin = 0;
  ret = state->m_pBuilder->FindPin(state->g_pMC, PINDIR_OUTPUT, &PIN_CATEGORY_STILL, 0, FALSE, 0, &pPin);
  if (SUCCEEDED(ret))
  {
   ret = pAMVidControl->SetMode(pPin, VideoControlFlag_Trigger);
   pPin->Release();
  }else{
   MessageBox(NULL, _T("error_10!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  }
  pAMVidControl->Release();
 }else
 {
  MessageBox(NULL, _T("error_9!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
 }
 
 ret = state->m_pBuilder->RenderStream(&PIN_CATEGORY_STILL,&MEDIATYPE_Video,state->pSrcFilter,state->m_pTSG_Filter,state->m_pNull);
 if(FAILED(ret))
 {
  MessageBox(NULL, _T("error_11!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1);
 }
 
 ret = state->g_pMC->Run(); 
 if (FAILED(ret))  
 { 
  MessageBox(NULL, _T("请检查该设备是否被占用!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return 0; 
 } 
 
 ret = state->pSrcFilter->QueryInterface(IID_IAMVideoControl, (void**)&state->m_pAMVideoControl);
 if (SUCCEEDED(ret))
 {
  IPin *pPin = NULL;
  ret = state->m_pBuilder->FindPin(state->pSrcFilter,PINDIR_OUTPUT,&PIN_CATEGORY_STILL,0,false,0,&pPin);
  if (SUCCEEDED(ret))
  {
   ret = state->m_pAMVideoControl->SetMode(pPin,VideoControlFlag_Trigger);
   pPin->Release();
   pPin = NULL;
  }
  else
  {
   MessageBox(NULL, _T("error_13!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
   return(-1);
  }
 }else
 {
  MessageBox(NULL, _T("error_14!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
  return(-1);
 }
 
 CSampleGrabberCB *CB = new CSampleGrabberCB(); 
 CB->cb_hwnd = state->hwnd; 
 CB->Width = DEFAULT_VIDEO_WIDTH; 
 CB->Height = DEFAULT_VIDEO_HEIGHT; 
 ret = state->m_pTSG->SetOneShot(FALSE);
 if (FAILED(ret))
 {
  MessageBox(NULL, _T("error_15!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
 }
 ret = state->m_pTSG->SetBufferSamples(TRUE);
 if (FAILED(ret))
 {
  MessageBox(NULL, _T("error_16!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
 }
 ret = state->m_pTSG->SetCallback(CB,1);
 if (FAILED(ret))
 {
  MessageBox(NULL, _T("error_17!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
 }
 ret = state->m_pTSG->GetConnectedMediaType(&state->g_StillMediaType);
 if (FAILED(ret))
 {
  MessageBox(NULL, _T("error_18!"), _T("系统提示"), MB_OK|MB_ICONINFORMATION); 
 }
 
//  ::SetWindowPos(state->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
//  ::SetWindowPos(state->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); 
 
 ShowWindow(state->hwnd, 1); 
 
 while(GetMessage(&msg,NULL,0,0)) 
 { 
  TranslateMessage(&msg); 
  DispatchMessage(&msg); 
 } 
 
 CoUninitialize(); 
 
 return 0; 
}