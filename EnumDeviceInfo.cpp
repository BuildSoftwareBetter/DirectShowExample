/**
 * 枚举输出计算机上的所有视频输入设备信息
 * 
 * 并输出视频设备的PIN脚相关信息
*/
#include <afxwin.h>
#include <tchar.h>
#include <dshow.h>
#include <iostream>
using namespace std;

void ShowFilterPinInfo(IBaseFilter *pDevFilter)
{
}

void ShowDeviceFilterInfo(IBaseFilter *pDevFilter)
{
    // 1. 显示设备的所有PIN信息
    // 2. 显示设备的所有视频输出分辨率、帧率
    // 3.
}

// 显示视频质量控制属性的范围值
void ShowDevicePropertyRange(IBaseFilter *pDevFilter)
{
    // 视频质量设置
    IAMVideoProcAmp *pProcAmp = NULL;
    HRESULT hr = pDevFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)&pProcAmp);
    if (FAILED(hr))
        return;

    // 获取指定属性的调整值范围，调整步长，默认值，手动/自动调整，是否支持该属性
    long min, max, step, deflt, flags;
    hr = pProcAmp->GetRange(VideoProcAmpProperty::VideoProcAmp_BacklightCompensation, &min, &max, &step, &deflt, &flags);
    if (hr == E_PROP_ID_UNSUPPORTED)
    {
        cout << "unsupport this property" << endl;
    }
    if (flags == VideoProcAmpFlags::VideoProcAmp_Flags_Auto) // 自动调整
    {
    }

    // 获取指定属性当前的值，及调整方式（手动/自动）
    long val;
    hr = pProcAmp->Get(VideoProcAmpProperty::VideoProcAmp_BacklightCompensation, &val, &flags);

    // 设置属性值，设置成手动调整
    hr = pProcAmp->Set(VideoProcAmpProperty::VideoProcAmp_BacklightCompensation, val, VideoProcAmpFlags::VideoProcAmp_Flags_Manual);

    pProcAmp->Release();

    // 相机控制
    IAMCameraControl *pCameraCtrl = NULL;
    hr = pDevFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraCtrl);

    // 获取曝光设置范围，调整步长，默认值，手动/自动调整，是否支持该属性
    hr = pCameraCtrl->GetRange(CameraControlProperty::CameraControl_Exposure, &min, &max, &step, &deflt, &flags);
    if (hr == E_PROP_ID_UNSUPPORTED)
    {
        cout << "unsupport this property" << endl;
    }

    
}

int main()
{
    // 初始化COM库
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        cout << "COM init failed " << hr << endl;
        return -1;
    }

    // 获取设备枚举COM组件
    ICreateDevEnum *pSysDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
    if (FAILED(hr))
    {
        cout << "Create System Device Enum Object Failed " << hr << endl;
        return -1;
    }

    // 创建视频输入设备类枚举对象
    IEnumMoniker *pEnumCat = NULL;
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
    if (FAILED(hr))
    {
        cout << "Create Video Input Device Enum Object Failed " << hr << endl;
        return -1;
    }

    // 获取设备信息
    IMoniker *pMoniker = NULL;
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
                _tprintf(_T("FriendlyName %ws \r\n"), varName.bstrVal);
                // printf("%S\n", varName.bstrVal);
            }
            VariantClear(&varName);

            // 获取封装设备的Filter
            IBaseFilter *pFilter;
            hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)&pFilter);

            pFilter->Release();
        }

        pMoniker->Release();
    }

    // 释放COM库
    CoUninitialize();
}