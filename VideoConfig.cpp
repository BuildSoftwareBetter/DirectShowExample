/**
 * 视频参数设置
 * 
 * VFW卡只能通过驱动定制的设置窗口设置
 * WDM卡可以通过IAMStreamConfig和IAMVideoProcAmp接口编程设置
 * 
 * 
 * IAMStreamConfig：设置帧率、视频格式（MJPEG/YUYV）、视频大小
 * IAMVideoProcAmp：设置增益、白平衡等
 * 
 * IAMCameraControl：相机控制，控制曝光
*/

#include <dshow.h>

int main()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
        return;

    IGraphBuilder *pGraph = NULL;
    ICaptureGraphBuilder2 *pBuild = NULL;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&pBuild);
    if (FAILED(hr))
        return;

    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
        return;

    // pBuild->

    // VIDEO_STREAM_CONFIG_CAPS
    // VideoProcAmpProperty
    // IAMCameraControl
}