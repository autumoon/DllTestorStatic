// DllTestorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DllTestor.h"
#include "DllTestorDlg.h"
#include "afxdialogex.h"

#include <algorithm>
#include "Shlwapi.h"

/*避免windows.h中的min/max宏干扰std::max/std::min*/
#undef min
#undef max

#pragma comment(lib, "shlwapi.lib")

#ifdef CMD_OUTPUT
#include <io.h>
#include <fcntl.h>
#include <iostream>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//进度RAII保护，确保异常时也能正常结束进度显示
class CProgressGuard
{
public:
	explicit CProgressGuard(CProgressInterface* p) : m_p(p)
	{
		if (m_p) m_p->Start();
	}
	~CProgressGuard()
	{
		if (m_p) m_p->End();
	}
private:
	CProgressInterface* m_p;
};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CDllTestorDlg 对话框

CDllTestorDlg::CDllTestorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDllTestorDlg::IDD, pParent)
	, m_hBgBitmap(NULL)
	, m_eProcessMode(ProcessMode::File)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDllTestorDlg::~CDllTestorDlg()
{
	if (m_hBgBitmap)
	{
		::DeleteObject(m_hBgBitmap);
		m_hBgBitmap = NULL;
	}
}

void CDllTestorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEMS, m_listItems);
	DDX_Control(pDX, IDC_EDIT_DIR, m_eDstDir);
}

_tstring CDllTestorDlg::GetIniPath(const TCHAR* szFileExt /*= _T(".ini")*/)
{
	std::vector<TCHAR> buffer(MAX_PATH, 0);

	DWORD dwLen = 0;
	while ((dwLen = GetModuleFileName(NULL, buffer.data(), (DWORD)buffer.size())) != 0)
	{
		if (dwLen < (DWORD)buffer.size())
		{
			break;
		}

		buffer.resize(buffer.size() * 2);
	}

	if (dwLen == 0)
	{
		return _tstring();
	}

	_tstring strModulePath = CMfcStrFile::CString2string(buffer.data());
	_tstring strIniPath = CStdStr::ReplaceSuffix(strModulePath, szFileExt);

	return strIniPath;
}

bool CDllTestorDlg::IsProperSuffix(const _tstring& strFilePath, const std::vector<_tstring>& vSuffix)
{
	//包含所有文件
	size_t nFSNum = vSuffix.size();

	for (size_t i = 0; i < nFSNum; ++i)
	{
		_tstring strCurSpe = vSuffix[i];
		if (strCurSpe == _T("*.*") || strCurSpe == _T("*"))
		{
			return true;
		}
	}

	//包含指定的suffix
	_tstring strCurSuffix = CStdStr::ToUpperLower(CStdStr::GetSuffixOfFile(strFilePath, false));
	for (size_t i = 0; i < nFSNum; ++i)
	{
		_tstring strCurSpe = CStdStr::ToUpperLower(CStdStr::GetSuffixOfFile(vSuffix[i], false));
		if (strCurSuffix == strCurSpe)
		{
			return true;
		}
	}

	return false;
}

int CDllTestorDlg::AddItemToList(_tstring stItemPath)
{
	if (stItemPath.empty())
	{
		return m_listItems.GetItemCount();
	}

	//规范化路径作为去重key
	TCHAR szFull[MAX_PATH] = {0};
	_tstring stKey = stItemPath;
	if (GetFullPathName(stItemPath.c_str(), MAX_PATH, szFull, NULL) > 0)
	{
		stKey = szFull;
	}

	//大小写不敏感去重
	if (m_itemSet.find(stKey) != m_itemSet.end())
	{
		return m_listItems.GetItemCount();
	}

	bool bAllow = false;
	if (m_eProcessMode == ProcessMode::File || m_eProcessMode == ProcessMode::Both)
	{
		if (CStdFile::IfAccessFile(stItemPath) && IsProperSuffix(stItemPath, m_cfg.vSuffixs))
		{
			bAllow = true;
		}
	}

	if (!bAllow && (m_eProcessMode == ProcessMode::Dir || m_eProcessMode == ProcessMode::Both))
	{
		if (PathIsDirectory(stItemPath.c_str()))
		{
			bAllow = true;
		}
	}

	if (bAllow)
	{
		int nPos = m_listItems.GetItemCount();
		m_listItems.InsertItem(nPos, stItemPath.c_str());
		m_itemSet.insert(stKey);
	}

	return m_listItems.GetItemCount();
}

bool CDllTestorDlg::ProcessFile(const _tstring& stSrcPath, const _tstring& stDstPath, config_s& _cfg)
{
	UNREFERENCED_PARAMETER(_cfg);
	return CStdFile::CopyAFile(stSrcPath, stDstPath, false);
}

void CDllTestorDlg::RebuildItemSet()
{
	m_itemSet.clear();

	int nItemNum = m_listItems.GetItemCount();
	for (int i = 0; i < nItemNum; ++i)
	{
		CString strCurItem = m_listItems.GetItemText(i, 0);
		_tstring stCurItem = CMfcStrFile::CString2string(strCurItem);

		TCHAR szFull[MAX_PATH] = {0};
		_tstring stKey = stCurItem;
		if (GetFullPathName(stCurItem.c_str(), MAX_PATH, szFull, NULL) > 0)
		{
			stKey = szFull;
		}

		m_itemSet.insert(stKey);
	}
}

BEGIN_EASYSIZE_MAP(CDllTestorDlg)
	EASYSIZE(IDOK,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_DSTFILE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_OPEN,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_FILE,ES_BORDER,ES_KEEPSIZE,IDC_BUTTON_OPEN,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_DSTDIR,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_BROWSE,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_DIR,ES_BORDER,ES_KEEPSIZE,IDC_BUTTON_BROWSE,ES_BORDER,0)
	EASYSIZE(IDC_LIST_ITEMS,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_ADD_ITEMS,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_DEL_ITEMS,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_CLEAR_ITEMS,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP

BEGIN_MESSAGE_MAP(CDllTestorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDllTestorDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CDllTestorDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ITEMS, &CDllTestorDlg::OnBnClickedButtonAddItems)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ITEMS, &CDllTestorDlg::OnBnClickedButtonDelItems)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ITEMS, &CDllTestorDlg::OnBnClickedButtonClearItems)
	ON_BN_CLICKED(IDOK, &CDllTestorDlg::OnBnClickedOk)
	ON_WM_DROPFILES()
	ON_WM_ERASEBKGND()
	ON_WM_HELPINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDllTestorDlg 消息处理程序

BOOL CDllTestorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//支持文件拖拽
	DragAcceptFiles(TRUE);
	m_eDstDir.DragAcceptFiles(TRUE);

	// TODO: 在此添加额外的初始化代码
	_tstring strItemName;
	switch (m_eProcessMode)
	{
	case ProcessMode::File:
	default:
		strItemName = _T("文件");
		break;
	case ProcessMode::Dir:
		strItemName = _T("目录");
		break;
	case ProcessMode::Both:
		strItemName = _T("项目");
		break;
	}

	//窗口名称
	_tstring strWindowName = _T("项目处理");
	SetWindowText(strWindowName.c_str());

	//设置AfxMessageBox的标题
	//First free the string allocated by MFC at CWinApp startup.
	//The string is allocated before InitInstance is called.
	free((void*)theApp.m_pszAppName);
	//Change the name of the application file.
	//The CWinApp destructor will free the memory.
	theApp.m_pszAppName=_tcsdup(strWindowName.c_str());

	//按钮名称
	SetDlgItemText(IDC_BUTTON_ADD_ITEMS, (_T("添加") + strItemName).c_str());
	SetDlgItemText(IDC_BUTTON_DEL_ITEMS, (_T("删除") + strItemName).c_str());

	_tstring strAddItemTips = _T("待处理的") + strItemName +  _T("(可拖拽添加):");

	CRect rListRect;
	m_listItems.GetClientRect(rListRect);
	m_listItems.InsertColumn(0, strAddItemTips.c_str(), LVCFMT_LEFT, rListRect.Width());

	//读取配置信息
	_tstring strIniPath = GetIniPath();
	if (CStdFile::IfAccessFile(strIniPath))
	{
		ReadIniFile(strIniPath, m_cfg);
	}
	else
	{
		WriteIniFile(strIniPath, m_cfg);
	}

	//只能拖拽单个目录
	m_eDstDir.SetFlag(EDIT_DIR_JUDGE | EDIT_SIG_JUDGE);

	if (m_cfg.bRemPath)
	{
		for (size_t i = 0; i < m_cfg.vItemPaths.size(); ++i)
		{
			AddItemToList(m_cfg.vItemPaths[i]);
		}

		if (m_cfg.vDstPaths.size() > 0)
		{
			SetDlgItemText(IDC_EDIT_DIR, VectorToString(m_cfg.vDstPaths).c_str());
		}
	}

	//如果是将文件夹拖拽到应用程序图标或者快捷方式
#ifdef CMD_INPUT
	int argc = 0;
#ifdef _UNICODE
	LPWSTR *argv=::CommandLineToArgvW(::GetCommandLine(),&argc);
#else
	LPWSTR *argv=::CommandLineToArgvW(CStdStr::s2ws(::GetCommandLine()).c_str(),&argc);
#endif

	if (argv != NULL)
	{
		CStringArray arrCmds;
		//去掉第一个程序自身的参数
		for (int i = 1; i < argc; ++i)
		{
#ifdef _UNICODE
			arrCmds.Add(argv[i]);
#else
			std::string sArc = CStdStr::ws2s(argv[i]);
			arrCmds.Add(sArc.c_str());
#endif
		}

		if (arrCmds.GetCount() >= 1)
		{
			int nItems = (int)arrCmds.GetCount();
			int nItemCount = 0;
			for (int i = 0; i < nItems; ++i)
			{
				CString strCurItem = arrCmds[i];
				_tstring sItem = CMfcStrFile::CString2string(strCurItem);
				if(PathFileExists(sItem.c_str()))
				{
					nItemCount = AddItemToList(sItem);
				}
			}
			CString strTmp[2];
			strTmp[0].LoadString(IDS_PROCESS_NOW);
			strTmp[1].LoadString(IDS_TIPS);
			//文件和文件夹同时成立时才会执行，可根据需要修改
			if (nItemCount && MessageBox(strTmp[0], strTmp[1], MB_YESNO) == IDYES)
			{
				OnBnClickedOk();
			}
		}

		LocalFree(argv);
	}
#endif // CMD_INPUT

	//加载背景位图
#ifdef DLG_BACKGROUND
	_tstring stBgPath = GetIniPath(_T(".bmp"));
	if (CStdFile::IfAccessFile(stBgPath))
	{
		m_hBgBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), stBgPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	if (!m_hBgBitmap)
	{
		m_hBgBitmap = ::LoadBitmap(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
	}
#endif // DLG_BACKGROUND

#ifdef CMD_OUTPUT
	SetCommandLine();
#endif // CMD_OUTPUT

	INIT_EASYSIZE;

	if (m_cfg.nWindowWidth < 640 || m_cfg.nWindowHeight < 480)
	{
		m_cfg.nWindowWidth = 640;
		m_cfg.nWindowHeight = 480;
	}
	SetWindowPos(NULL, 0, 0, m_cfg.nWindowWidth, m_cfg.nWindowHeight,
		SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	CenterWindow();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDllTestorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
// 来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
// 这将由框架自动完成。

void CDllTestorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDllTestorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDllTestorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

#ifdef CMD_OUTPUT
	ReleaseCommandLine();
#endif
}

#ifdef CMD_OUTPUT
int CDllTestorDlg::SetCommandLine()
{
	AllocConsole();
	*stdin  = *( _fdopen(_open_osfhandle((intptr_t)::GetStdHandle(STD_INPUT_HANDLE), _O_TEXT), "r"));
	*stdout = *( _fdopen(_open_osfhandle((intptr_t)::GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT), "wt"));
	std::ios_base::sync_with_stdio();
	return 0;
}

int CDllTestorDlg::ReleaseCommandLine()
{
	FreeConsole();
	return 0;
}
#endif

void CDllTestorDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFilePath = CMfcStrFile::SaveSuffixFile(_T(".txt"));

	if (strFilePath.GetLength() > 0)
	{
		SetDlgItemText(IDC_EDIT_FILE, strFilePath);
		CString strTmp[2];
		strTmp[0].LoadString(IDS_PROCESS_NOW);
		strTmp[1].LoadString(IDS_TIPS);
		if (MessageBox(strTmp[0], strTmp[1], MB_YESNO) == IDYES)
		{
			OnBnClickedOk();
		}
	}
}

void CDllTestorDlg::OnBnClickedButtonBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strDirPath = CMfcStrFile::BrowseDir(true);

	if (strDirPath.GetLength() > 0)
	{
		SetDlgItemText(IDC_EDIT_DIR, strDirPath);
	}
}

void CDllTestorDlg::OnBnClickedButtonAddItems()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringArray arrItems;

	if (m_eProcessMode == ProcessMode::File || m_eProcessMode == ProcessMode::Both)
	{
		CMfcStrFile::OpenMultiFiles(arrItems, 0);
	}
	else
	{
		CString strDir = CMfcStrFile::BrowseDir();
		if (strDir.GetLength() > 0)
		{
			arrItems.Add(strDir);
		}
	}

	INT_PTR nCount = arrItems.GetCount();
	for (INT_PTR i = 0; i < nCount; ++i)
	{
		_tstring stItem = CMfcStrFile::CString2string(arrItems[i]);
		AddItemToList(stItem);
	}
}

void CDllTestorDlg::OnBnClickedButtonDelItems()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION nPos = m_listItems.GetFirstSelectedItemPosition();
	while (nPos)
	{
		int iSelItem = m_listItems.GetNextSelectedItem(nPos);
		m_listItems.DeleteItem(iSelItem);       
		nPos = m_listItems.GetFirstSelectedItemPosition();
	}
	RebuildItemSet();
}


void CDllTestorDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	const int nMaxPathLength = 2048;
	std::vector<TCHAR> szItemPath(nMaxPathLength, 0);
	// 通过设置iFiles参数为0xFFFFFFFF,可以取得当前拖动的文件数量，
	// 当设置为0xFFFFFFFF,函数间忽略后面两个参数。
	UINT nNum = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	// 通过循环依次取得拖动文件的File Name信息，并把它添加到ListBox中
	for (UINT i = 0; i < nNum; ++i)
	{
		DragQueryFile(hDropInfo, i, szItemPath.data(), nMaxPathLength);
		if (PathFileExists(szItemPath.data()) == TRUE)
		{
			AddItemToList(CMfcStrFile::CString2string(szItemPath.data()));
		}
	}

	// 结束此次拖拽操作，并释放分配的资源
	DragFinish(hDropInfo);
}

BOOL CDllTestorDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

#ifndef DLG_BACKGROUND
	return CDialogEx::OnEraseBkgnd(pDC);
#else

	if (m_hBgBitmap)
	{
		//获取位图尺寸
		BITMAP bitmap;
		GetObject(m_hBgBitmap, sizeof (BITMAP), &bitmap);

		//获取对话框尺寸
		CRect rect;
		GetClientRect(&rect);

		//创建DC
		HDC hBkDC = ::CreateCompatibleDC(pDC->m_hDC);
		if (hBkDC)
		{
			//选择位图并保存旧位图
			HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hBkDC, m_hBgBitmap);

			//设置拉伸模式，避免拉伸后出现黑边
			::SetStretchBltMode(pDC->m_hDC, HALFTONE);

			//绘图
			::StretchBlt(pDC->m_hDC, 0, 0, rect.Width(), rect.Height(), hBkDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

			//恢复旧位图并清理DC
			::SelectObject(hBkDC, hOldBitmap);
			::DeleteDC(hBkDC);
		}
	}
	else
	{
		CDialogEx::OnEraseBkgnd(pDC);
	}

	//这个很重要
	return TRUE;
#endif // DLG_BACKGROUND
}

BOOL CDllTestorDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (MessageBox(_T("如果遇到任何问题需要帮助，\n请通过邮箱autumoon@vip.qq.com联系我!\n\n立即复制“autumoon@vip.qq.com”到剪贴板吗？"), 
		_T("需要帮助"), MB_YESNO) == IDYES)
	{
		//已经复制到剪贴板
		CString strCopy(_T("autumoon@vip.qq.com"));
		SIZE_T nBytes = (strCopy.GetLength() + 1) * sizeof(TCHAR);
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, nBytes);
if (hMem != NULL)
		{
			LPTSTR lpBuffer = (LPTSTR)GlobalLock(hMem);
			if (lpBuffer == NULL)
			{
				GlobalFree(hMem);
				return TRUE;
			}

			memcpy(lpBuffer, strCopy.GetString(), nBytes);
			GlobalUnlock(hMem);

			if (!::OpenClipboard(m_hWnd))
			{
				GlobalFree(hMem);
				return TRUE;
			}

			EmptyClipboard();
#ifdef _UNICODE
			if (SetClipboardData(CF_UNICODETEXT, hMem) == NULL)
#else
			if (SetClipboardData(CF_TEXT, hMem) == NULL)
#endif // _UNICODE
			{
				GlobalFree(hMem);
			}
			CloseClipboard();
			MessageBox(_T("“autumoon@vip.qq.com”\n  已经成功复制到剪贴板！"), _T("地址复制完成!"), MB_ICONINFORMATION);
		}
	}

	return TRUE;
}

void CDllTestorDlg::OnBnClickedButtonClearItems()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_listItems.GetItemCount())
	{
		m_listItems.DeleteAllItems();
	}
	m_itemSet.clear();
}

void CDllTestorDlg::OnBnClickedOk()
{
	// TODO:  在此添加命令处理程序代码
	CString strDstDir;
	GetDlgItemText(IDC_EDIT_DIR, strDstDir);

	if (!strDstDir.GetLength())
	{
		strDstDir = CMfcStrFile::BrowseDir(true);
		if (strDstDir.GetLength() > 0)
		{
			SetDlgItemText(IDC_EDIT_DIR, strDstDir);
		}
		else
		{
			return;
		}
	}

	std::vector<_tstring> vItems;
	const size_t nListCount = (size_t)m_listItems.GetItemCount();
	vItems.reserve(nListCount);
	for (size_t i = 0; i < nListCount; ++i)
	{
		CString strCurItem = m_listItems.GetItemText((int)i, 0);
		_tstring stCurItem = CMfcStrFile::CString2string(strCurItem);
		vItems.push_back(stCurItem);
	}

	//若没有添加任何待处理文件则提示并返回
	if (vItems.empty())
	{
		AfxMessageBox(_T("请先添加待处理文件"));
		return;
	}

	const size_t nItemCount = vItems.size();
	_tstring stDstDir = CMfcStrFile::CString2string(strDstDir);
	if (!CStdDir::IfAccessDir(stDstDir) && !CStdDir::CreateDir(stDstDir))
	{
		return;
	}

	//保存配置文件
	m_cfg.vItemPaths = vItems;
	m_cfg.vDstPaths.clear();
	m_cfg.vDstPaths.push_back(stDstDir);
	WriteIniFile(GetIniPath(), m_cfg);

	//开始显示进度
	CTaskBarProgress tbp(m_hWnd);
	CProgressInterface* ppi = &tbp;
	CProgressGuard guard(&tbp);
	CElapsedTime et;

	int nSuccess = 0, nFail = 0;

	try
	{
		//记录日志
		//CLOG::Out(_T("%s"), _T("start task!"));
		//记录耗时
		et.Begin();
		/*********************************这里增加主程序 开始***************************************/
		//处理所有项目，推荐判断项目是否存在
		stDstDir = CStdStr::AddSlashIfNeeded(stDstDir);
		for (size_t i = 0; i < nItemCount; ++i)
		{
			const _tstring stCurItem = vItems[i];
			const _tstring stDstItem = stDstDir + CStdStr::GetNameOfFile(stCurItem);

			if (m_eProcessMode == ProcessMode::File || m_eProcessMode == ProcessMode::Both)
			{
				if (CStdFile::IfAccessFile(stCurItem) && IsProperSuffix(stCurItem, m_cfg.vSuffixs))
				{
					if (ProcessFile(stCurItem, stDstItem, m_cfg))
					{
						++nSuccess;
					}
					else
					{
						++nFail;
					}
				}
			}

			if (m_eProcessMode == ProcessMode::Dir || m_eProcessMode == ProcessMode::Both)
			{
				//如果是目录
				if (PathIsDirectory(stCurItem.c_str()))
				{
					//处理文件夹
					std::vector<_tstring> vSubFiles;
					size_t nSubFileCount = getFiles(stCurItem, vSubFiles, m_cfg.vSuffixs, true);

					for (size_t j = 0; j < nSubFileCount; ++j)
					{
						_tstring stSubSrcFile = vSubFiles[j];
						TCHAR szRelative[MAX_PATH] = {0};
						if (PathRelativePathTo(szRelative,
							stCurItem.c_str(),
							FILE_ATTRIBUTE_DIRECTORY,
							stSubSrcFile.c_str(),
							FILE_ATTRIBUTE_NORMAL))
						{
							_tstring stRel = szRelative;
							if (stRel.size() >= 2 && stRel[0] == _T('.') &&
								(stRel[1] == _T('\\') || stRel[1] == _T('/')))
							{
								stRel = stRel.substr(2);
							}
							_tstring strSubDstFile = CStdStr::AddSlashIfNeeded(stDstItem) + stRel;
							_tstring strSubDir = CStdStr::GetDirOfFile(strSubDstFile);
							if (!CStdDir::IfAccessDir(strSubDir) && !CStdDir::CreateDir(strSubDir))
							{
								++nFail;
								continue;
							}
							if (ProcessFile(stSubSrcFile, strSubDstFile, m_cfg))
							{
								++nSuccess;
							}
							else
							{
								++nFail;
							}
						}
						else
						{
							++nFail;
						}
					}
				}
			}

			ppi->SetProgressValue((int)(i + 1), (int)nItemCount);
		}
		/*********************************这里增加主程序 结束***************************************/
	}
	catch (CException* e)
	{
		e->Delete();
		AfxMessageBox(_T("处理过程中发生 MFC 异常"));
	}
	catch (std::exception& e)
	{
		CString str;
		str.Format(_T("处理过程中发生异常：%S"), e.what());
		AfxMessageBox(str);
	}
	catch (...)
	{
		AfxMessageBox(_T("处理过程中发生未知异常"));
	}

	//结束耗时
	int nMin = 0, nSecond = 0, nMilliSecond = 0;
	et.End(nMin, nSecond, nMilliSecond);
	//结束日志
	//CLOG::Out(_T("%s"),_T("end task!"));
	//CLOG::Out(_T("This task costs %d min %d second %d millisecond!"), nMin, nSecond, nMilliSecond);
	//CLOG::End();

	FlashWindow(TRUE);

#ifdef DLG_ELAPSED_TIME
	CString strTips;
	strTips.Format(_T("本次耗时 %d分%d秒%d毫秒!\n成功 %d 个，失败 %d 个"), nMin, nSecond, nMilliSecond, nSuccess, nFail);
	AfxMessageBox(strTips);
#else
	CString strTips;
	strTips.Format(_T("处理完成!\n成功 %d 个，失败 %d 个"), nSuccess, nFail);
	AfxMessageBox(strTips);
#endif // DLG_ELAPSED_TIME
}

void CDllTestorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	UPDATE_EASYSIZE;

	//改变ListCtrl表的宽度
	CRect rcList;
	CWnd* pWnd = GetDlgItem(IDC_LIST_ITEMS);
	if (pWnd)
	{
		pWnd->GetWindowRect(&rcList);
		m_listItems.SetColumnWidth(0, std::max(10, rcList.Width() - 6));
	}

	if (nType != SIZE_MINIMIZED)
	{
		CRect rcDlg;
		GetClientRect(rcDlg);
		InvalidateRect(rcDlg);
		//记录窗口整体大小（含标题栏和边框）
		CRect rcWnd;
		GetWindowRect(&rcWnd);
		m_cfg.nWindowWidth = rcWnd.Width();
		m_cfg.nWindowHeight = rcWnd.Height();
	}
}
