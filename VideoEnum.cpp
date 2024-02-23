#include <dshow.h>
#include <iostream>

using namespace std;

const char buf[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

typedef struct sGUID_STR
{
    char str[36];
} GUID_STR;

GUID_STR GetGUID(GUID guid)
{
    GUID_STR gs;
    gs.str[35] = 0;
    sprintf(gs.str, "%X", guid.Data1);

    gs.str[8] = '-';
    // sprintf(gs.str + 9, "%2X", guid.Data2);

    gs.str[9] = buf[0xF & (guid.Data2 >> 12)];
    gs.str[10] = buf[0xF & (guid.Data2 >> 8)];
    gs.str[11] = buf[0xF & (guid.Data2 >> 4)];
    gs.str[12] = buf[0xF & (guid.Data2)];

    gs.str[13] = '-';
    // sprintf(gs.str + 14, "%2X", guid.Data3);
    gs.str[14] = buf[0xF & (guid.Data3 >> 12)];
    gs.str[15] = buf[0xF & (guid.Data3 >> 8)];
    gs.str[16] = buf[0xF & (guid.Data3 >> 4)];
    gs.str[17] = buf[0xF & (guid.Data3)];

    gs.str[18] = '-';

    // cout << guid.Data1 << "-" << guid.Data2 << "-" << guid.Data3 << "-{";
    for (int i = 0; i < 8; i++)
    {
        // sprintf(gs.str + 19 + 2 * i, "%X", guid.Data4[i]);

        gs.str[19 + i * 2] = buf[0xF & (guid.Data4[i] >> 4)];
        gs.str[19 + i * 2 + 1] = buf[0xF & (guid.Data4[i])];

        // cout << guid.Data4[i] << "-";
    }

    // cout << "}" << endl;

    return gs;
}

// VideoInfo

// void ShowVideoInfo(AM_MEDIA_TYPE * pmt)
// {
//     VideoInfo
// }

// Enum Pin Media Type

HRESULT GetPinMediaType(IPin *pPin)
{
    IEnumMediaTypes *pEnum = NULL;
    AM_MEDIA_TYPE *pmt = NULL;
    BOOL bFound = FALSE;

    HRESULT hr = pPin->EnumMediaTypes(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }

    while (hr = pEnum->Next(1, &pmt, NULL), hr == S_OK)
    {
        // MEDIATYPE_Audio

        cout << "majorType : " << GetGUID(pmt->majortype).str << ",subType : " << GetGUID(pmt->subtype).str << ",formatType : " << GetGUID(pmt->formattype).str << endl;
    }

    pEnum->Release();
    pEnum = NULL;

    if (SUCCEEDED(hr))
    {
        if (!bFound)
        {
            hr = VFW_E_NOT_FOUND;
        }
    }
    return hr;
}

// Enum Pins
HRESULT EnumPins(IBaseFilter *pFilter)
{
    IEnumPins *pEnum = NULL;
    IPin *pPin = NULL;
    HRESULT hr;

    hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
    while (pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        hr = pPin->QueryDirection(&PinDirThis);
        if (FAILED(hr))
        {
            cout << "no pins" << endl;

            pPin->Release();
            pEnum->Release();
            return hr;
        }

        cout << "PIN Drection: " << PinDirThis << endl;

        GetPinMediaType(pPin);

        cout << __LINE__ << endl;
        // pPin->

        // if (PinDir == PinDirThis)
        // {
        //     // Found a match. Return the IPin pointer to the caller.
        //     *ppPin = pPin;
        //     pEnum->Release();
        //     return S_OK;
        // }
        // Release the pin for the next time through the loop.
        pPin->Release();
    }

    cout << __LINE__ << endl;
    // No more pins. We did not find a match.
    pEnum->Release();
    return E_FAIL;
}

int main()
{
    cout << "something " << endl;
    HRESULT hr = CoInitialize(NULL);

    cout << SUCCEEDED(hr) << endl;

    cout << __LINE__ << endl;
    ICreateDevEnum *pSysDevEnum = NULL;
    // 创建并默认初始化一个指定CLSID关联的类的对象
    // 创建SystemDeviceEnum对象
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
    cout << __LINE__ << endl;
    cout << SUCCEEDED(hr) << endl;

    IEnumMoniker *pEnumCat = NULL;
    // 创建关联的设备类型枚举对象
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

    cout << hr << endl;

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

            IBaseFilter *pFilter;
            // 获取封装设备的Filter
            hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)&pFilter);

            EnumPins(pFilter);

            pPropBag->Release();
        }
    }

    pEnumCat->Release();
    pSysDevEnum->Release();

    CoUninitialize();
}