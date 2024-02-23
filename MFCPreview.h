#include <afxwin.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
using namespace std;

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



int show();