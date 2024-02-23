#include <iostream>
using namespace std;

#include <dshow.h>

int main()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        // init COM failed
        cout << "init COM failed" << endl;
        return;
    }

    // 创建Filter Graph Manager对象
    IGraphBuilder *pGraph;
    // 查找类标识为CLSID_FilterGraph的COM组件，使用改组件创建接口标识符为IID_IGraphBuilder的进程内组件对象pGraph
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        // 创建 Filter Graph
        return;
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

    // 创建filter graph
    hr = pGraph->RenderFile(L"1.avi", NULL);

    // Filter Graph Manager MediaControl Run
    pControl->Run();
    long evCode = 0;
    
    // Filter Graph Manager MediaEvent Wait Event
    pEvent->WaitForCompletion(INFINITE, &evCode);

    pControl->Release();
    pEvent->Release();
    pGraph->Release();
    CoUninitialize();
}