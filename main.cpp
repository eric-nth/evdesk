#include <windows.h>
#include <tchar.h>


//全局变量
TCHAR szAppName[]=TEXT("EVDESK");
TCHAR szVDesk[]=TEXT("EVirtualDesktop");
HWND hwnd;
HDESK hVirtualDesk,hCurrentDesk,defaultDesk;
HANDLE explorerInfo;
 
//窗口回调函数
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
//创建虚拟窗口
VOID CreateVirtualDesk();
 
//在虚拟桌面创建一个进程
HANDLE ShellCreateInVDesk(PTSTR szName);

int WINAPI WinMain(HINSTANCE hIns,HINSTANCE,PSTR szCmd,int nShow)
{
	//创建窗口类型
	WNDCLASS wndclass;
	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc=WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance=hIns;
	wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName=NULL;
	wndclass.lpszClassName=szAppName;
 
	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL,TEXT("Register Failed!"),szAppName,MB_ICONERROR);
		return 0;
	}
 
	hwnd =CreateWindow(szAppName,szAppName,//类名
						WS_OVERLAPPEDWINDOW,//风格
						CW_USEDEFAULT,CW_USEDEFAULT,//左上角坐标
						CW_USEDEFAULT,CW_USEDEFAULT,//窗体的宽和高
						NULL,NULL,hIns,NULL);
	//ShowWindow(hwnd,nShow);
	//UpdateWindow(hwnd);
 
	//注册热键
	RegisterHotKey(hwnd,0x0001,MOD_CONTROL,'B');
	RegisterHotKey(hwnd,0x0002,0,VK_F7);
	//保存原始桌面句柄
	defaultDesk=GetThreadDesktop(GetCurrentThreadId());
	//创建虚拟桌面
	CreateVirtualDesk();
	TCHAR szProcess[]=TEXT("userinit");
	explorerInfo=ShellCreateInVDesk(szProcess);
	MSG msg;
	while(GetMessage(&msg,hwnd,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
 
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		hCurrentDesk=defaultDesk;
		return 0;
	case WM_HOTKEY:
		{
			switch(wParam)
			{
			case 0x0001:
				{
					if (hCurrentDesk == hVirtualDesk)
					{
						SetThreadDesktop(defaultDesk); 
						SwitchDesktop(defaultDesk);
						hCurrentDesk =defaultDesk;
					}
					else
					{
						SetThreadDesktop(hVirtualDesk); 
						SwitchDesktop(hVirtualDesk);
						hCurrentDesk =hVirtualDesk;
					}
				}
				break;
			case 0x0002:
				{
					SetThreadDesktop(defaultDesk); 
					SwitchDesktop(defaultDesk);
					TerminateProcess(explorerInfo,1);
					CloseDesktop(hVirtualDesk);
					//关闭虚拟桌面
					SendMessage(hwnd,WM_DESTROY,0,0);
					MessageBox(NULL, "虚拟桌面已关闭！", "evdesk", MB_OK);
				}
				break;
			}
			break;
		}
		return 0;
	case WM_QUERYENDSESSION: {
		MessageBox(NULL, "关机已阻止。\n重要提示: 请先不要点下面的“确定”，然后等出来“强制关机”“取消”的选项后，选择“取消”，然后关机就取消了，这时候再点“确定”即可阻止关机。", "evdesk", MB_OK);
		while(1) {
			Sleep(1000);
		}
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
 
HANDLE ShellCreateInVDesk(PTSTR szName)
{
	STARTUPINFO si={0};
	si.cb=sizeof(si);
	si.lpDesktop=szVDesk;
	PROCESS_INFORMATION pi;
	if ( !CreateProcess(NULL,szName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) )
	{
 
		MessageBox(NULL,TEXT("启动桌面失败！"),TEXT("Error"),0);
		ExitProcess(1);
	}
	return pi.hProcess;
}
 
VOID CreateVirtualDesk()
{
	hVirtualDesk  =CreateDesktop(szVDesk,
								NULL,NULL,//保留参数
								DF_ALLOWOTHERACCOUNTHOOK,
								GENERIC_ALL,
								NULL);
	MessageBox(NULL, "EVDESK Go!\nCtrl+B切换桌面!\n\nEvdesk2.0新功能: 阻止老师关你电脑。注意: 如果自己要关机，要么按电源键10秒断电关机，要么先运行evdesk.stop.cmd再关机，否则也会被evdesk拦截。同时请注意不要多次运行evdesk，否则可能会出问题。如果真的要重复打开，请先运行evdesk.stop.cmd。\n本软件仅供学习参考，请勿用于非法用途。本软件遵循Apache-2.0开源协议，使用本软件造成的一切后果由使用者负责。", "evdesk", MB_OK);
}
