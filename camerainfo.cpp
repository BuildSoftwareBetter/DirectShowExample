#include "stdafx.h" 
#include "USBPort.h" 
#include "USBPortDlg.h" 
#include "setupapi.h" 
#include <hidsdi.h> 
#ifdef _DEBUG 
#define new DEBUG_NEW 
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__; 
#endif 
........ 
........ 
HCURSOR CUSBPortDlg::OnQueryDragIcon() 
{ 
return (HCURSOR) m_hIcon; 
} 

void CUSBPortDlg::OnSearch() 
{ 
// TODO: Add your control notification handler code here 
m_strLog.Empty(); 

m_strLog = _T("正在查找系统中HID类的GUID标识.../r/n"); 
SetDlgItemText(IDC_EDIT_REPORT,m_strLog); 

// 查找本系统中HID类的GUID标识 
GUID guidHID; 
HidD_GetHidGuid(&guidHID); 
m_strLog += _T("HID类的GUID标识为/r/n"); 

CString strShow; 
strShow.Format("%08x-%04x-%04x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x/r/n", 
guidHID.Data1,guidHID.Data2,guidHID.Data3,guidHID.Data4[0], 
guidHID.Data4[1],guidHID.Data4[2],guidHID.Data4[3],guidHID.Data4[4], 
guidHID.Data4[5],guidHID.Data4[6],guidHID.Data4[7]); 

m_strLog += strShow; 
SetDlgItemText(IDC_EDIT_REPORT,m_strLog); 

// 准备查找符合HID规范的USB设备 
HDEVINFO hDevInfo = SetupDiGetClassDevs(&guidHID,NULL,0, 
DIGCF_PRESENT ¦ DIGCF_DEVICEINTERFACE); 

UINT nIndex = 0; 
m_strLog += _T("正在查找可用的USB设备.../r/n"); 
SetDlgItemText(IDC_EDIT_REPORT,m_strLog); 

// 查找USB设备接口 
SP_DEVICE_INTERFACE_DATA strtInterfaceData; 
BOOL bSuccess = SetupDiEnumDeviceInterfaces(hDevInfo,NULL,&guidHID,0, 
&strtInterfaceData); 
if (!bSuccess) 
{ 
m_strLog += _T("抱歉，未找到可用的USB设备"); 
SetupDiDestroyDeviceInfoList(hDevInfo); 
return; 
} 

// 若找到了设备，则读取设备路径名 
SP_DEVICE_INTERFACE_DETAIL_DATA strtDetailData; 
if (!SetupDiGetDeviceInterfaceDetail(hDevInfo,&strtInterfaceData, 
&strtDetailData,sizeof(strtDetailData),NULL,NULL)) 
{ 
AfxMessageBox(_T("查找设备路径时出错!")); 
SetupDiDestroyDeviceInfoList(hDevInfo); 
return; 
} 

// 找到了设备路径 
m_strPath = strtDetailData.DevicePath; 
strShow.Format("可以通过路径%s访问设备/r/n",m_strPath); 
m_strLog += strShow; 
SetDlgItemText(IDC_EDIT_REPORT,m_strLog); 

// 开放与设备的通信 
HANDLE hCom = CreateFile(m_strPath,GENERIC_READ ¦ GENERIC_WRITE, 
0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); 
if (hCom = INVALID_HANDLE_VALUE) 
{ 
AfxMessageBox(_T("无法打开通信端口")); 
SetupDiDestroyDeviceInfoList(hDevInfo); 
return; 
} 

// 查询设备标识 
HIDD_ATTRIBUTES strtAttrib; 
m_strPath += _T("正在读取设备的标识.../r/n"); 
SetDlgItemText(IDC_EDIT_REPORT,m_strPath); 

if (!HidD_GetAttributes(hCom,&strtAttrib)) 
{ 
AfxMessageBox(_T("查询设备状态时出错!")); 
CloseHandle(hCom); 
SetupDiDestroyDeviceInfoList(hDevInfo); 
} 

// 将有关该设备的标识显示出来 
m_strPath += _T("已读取，见右边所示/r/n"); 
SetDlgItemText(IDC_EDIT_REPORT,m_strPath); 

// 显示供应商标识 
strShow.Format("0x%x",strtAttrib.VendorID); 
SetDlgItemText(IDC_ID_VENDOR,strShow); 

// 显示产品标识 
strShow.Format("0x%x",strtAttrib.ProductID); 
SetDlgItemText(IDC_ID_PRODUCT,strShow); 

// 显示产品版本号 
strShow.Format("%d",strtAttrib.VersionNumber); 
SetDlgItemText(IDC_ID_VERSION,strShow); 

// 释放资源 
CloseHandle(hCom); 
SetupDiDestroyDeviceInfoList(hDevInfo); 
}
