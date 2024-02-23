DirectShow播放USB Camera视频及抓图的实现

1. Video Capture Filter
2. MPEG Decode Filter
3. Video Renderer Filter

参考：
- https://blog.csdn.net/lwwl12/article/details/109814229
- [DirectShow实现USB相机预览和抓图例子（使用ICaptureGraphBuilder2）](https://blog.csdn.net/lwwl12/article/details/110087109)
- [DirectShow实现USB相机预览和抓图例子（手动构建filter链）](https://blog.csdn.net/lwwl12/article/details/110223158)

*******

目标：

1. 视频设备查询，并列举所有的视频设备信息及支持的输出分辨率
2. 使用CLSID_CaptureGraphBuilder2构建最简单的视频播放程序，使用有窗模式，增加filter graph event
3. 在2之上增加抓图功能
4. 在3之上增加输出分辨率设置
5. 在4至少增加增益、白平衡等视频输出控制
6. 在5之上增加曝光等相机控制
7. 使用CLSID_CaptureGraphBuilder2构建无窗模式的视频播放程序
8. 不使用CLSID_CaptureGraphBuilder2，手动构建有窗模式视频播放程序


*******

编译：

```bash
#窗口模式，自动连接：
cl MFC.cpp MFCPreview.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

#窗口模式，手动连接 + sample grabber + smart tee：
cl CreateWnd.cpp ManulConn_Wnd.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

#自动创建窗口，手动连接 + sample grabber + smart tee，没有处理窗口消息导致阻塞，设置格式：
cl ManulConn.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

#自动创建窗口，手动连接 + sample grabber + smart tee，没有处理窗口消息导致阻塞，查找格式并设置：
cl preview.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

#窗口模式，自动连接 + sample grabber + smart tee，不卡：
cl preview_app.cpp preview_wnd.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

# sample grabber抓图
cl StillImage.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

cl StillImage2.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

# 枚举UVC设备
cl VideoEnum.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

#控件显示：
cl PreviewCtrl.cpp PreviewOnCtrl.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

#控件显示 + sample grabber，不卡，最好的一个demo：
cl preview_app_ctrl.cpp preview_wnd.cpp SaveGraphFile.cpp Strmiids.lib ole32.lib OleAut32.lib user32.lib

```