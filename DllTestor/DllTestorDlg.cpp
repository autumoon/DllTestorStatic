// DllTestorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DllTestor.h"
#include "DllTestorDlg.h"
#include "afxdialogex.h"

/*同时支持处理文件和目录*/
#if (defined ITEM_ONLY_DIR) && (defined ITEM_ONLY_FILE)
#define ITEM_DIR_FILE
#endif

/*防止同时未定义*/
#if (!defined ITEM_ONLY_DIR) && (!defined ITEM_ONLY_FILE)
#error "One of ITEM_ONLY_DIR or ITEM_ONLY_FILE must be defined!"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDllTestorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEMS, m_listItems);
	DDX_Control(pDX, IDC_EDIT_DIR, m_eDstDir);
}

_tstring CDllTestorDlg::GetIniPath(const TCHAR* szFileExt /*= _T(".ini")*/)
{
	TCHAR chpath[MAX_PATH];
	GetModuleFileName(NULL, chpath, sizeof(chpath));

	_tstring strModulePath = CMfcStrFile::CString2string(chpath);
	_tstring strIniPath = CStdStr::ReplaceSuffix(strModulePath, szFileExt);

	return strIniPath;
}

bool CDllTestorDlg::IsProperSuffix(const _tstring& strFilePath, const std::vector<_tstring>& vSuffix)
{
	//包含所有文件
	bool bGetAll = false;
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
	std::vector<_tstring> vItems;
	int nItemNum = m_listItems.GetItemCount();
	for (int i = 0; i < nItemNum; ++i)
	{
		CString strCurItem = m_listItems.GetItemText(i, 0);
		_tstring stCurItem = CMfcStrFile::CString2string(strCurItem);
		vItems.push_back(stCurItem);
	}

	if (CStdTpl::VectorContains(vItems, stItemPath))
	{
		return nItemNum;
	}

#ifdef ITEM_ONLY_FILE
	if (CStdFile::IfAccessFile(stItemPath) && IsProperSuffix(stItemPath, m_cfg.vSuffixs))
	{
		int nPos = m_listItems.GetItemCount();
		m_listItems.InsertItem(nPos, stItemPath.c_str());
	}
#endif // ITEM_ONLY_FILE

#ifdef ITEM_ONLY_DIR
	if(PathIsDirectory(stItemPath.c_str()))
	{
		int nPos = m_listItems.GetItemCount();
		m_listItems.InsertItem(nPos, stItemPath.c_str());
	}
#endif // ITEM_ONLY_DIR

	return m_listItems.GetItemCount();
}

int CDllTestorDlg::ProcessFile(const _tstring& stSrcPath, const _tstring& stDstPath, config_s& _cfg)
{
	CStdFile::CopyAFile(stSrcPath, stDstPath, false);

	return 0;
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

	// TODO: 在此添加额外的初始化代码
#ifdef ITEM_DIR_FILE
	_tstring strItemName = _T("项目");
#else

#ifdef ITEM_ONLY_DIR
	_tstring strItemName = _T("目录");
#else
	_tstring strItemName = _T("文件");
#endif

#endif

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
	LocalFree(argv);

	if (arrCmds.GetCount() >= 1)
	{
		int nItems = (int)arrCmds.GetCount();
		CString strFiles, strDirs;
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
#endif // CMD_INPUT

#ifdef CMD_OUTPUT
	SetCommandLine();
#endif // CMD_OUTPUT

	INIT_EASYSIZE;

	if (m_cfg.nWindowWidth < 640 || m_cfg.nWindowHeight < 480)
	{
		m_cfg.nWindowWidth = 640;
		m_cfg.nWindowHeight = 480;
	}
	SetWindowPos(&wndBottom,0,0,m_cfg.nWindowWidth,m_cfg.nWindowHeight, SWP_SHOWWINDOW);
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
	int nPos = m_listItems.GetItemCount();
	CStringArray arrItems;
#if (defined ITEM_DIR_FILE) || (defined ITEM_ONLY_FILE)

	size_t nCount = CMfcStrFile::OpenMultiFiles(arrItems, 0);

#else
	CString strDir = CMfcStrFile::BrowseDir();
	arrItems.Add(strDir);
	size_t nCount = arrItems.GetCount();
#endif // 

	if (nCount > 0)
	{
		for (size_t i = 0; i < nCount; ++i)
		{
			_tstring stItem = CMfcStrFile::CString2string(arrItems[i]);
			AddItemToList(stItem);
		}
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
}


void CDllTestorDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 定义一个缓冲区来存放读取的文件名信息
	TCHAR* szItemPath = nullptr;
	const int nMaxPathLength = 2048;
	CStdTpl::NewSafely(szItemPath, nMaxPathLength, true);
	// 通过设置iFiles参数为0xFFFFFFFF,可以取得当前拖动的文件数量，
	// 当设置为0xFFFFFFFF,函数间忽略后面两个参数。
	UINT nNum = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	// 通过循环依次取得拖动文件的File Name信息，并把它添加到ListBox中
	for (UINT i = 0; i < nNum; ++i)
	{
		DragQueryFile(hDropInfo, i, szItemPath, nMaxPathLength);
		if (PathFileExists(szItemPath) == TRUE)
		{
			AddItemToList(CMfcStrFile::CString2string(szItemPath));
		}
	}

	// 结束此次拖拽操作，并释放分配的资源
	CStdTpl::DelPointerSafely(szItemPath, true);
	DragFinish(hDropInfo);

	CDialogEx::OnDropFiles(hDropInfo);
}

BOOL CDllTestorDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnEraseBkgnd(pDC);

#ifdef DLG_BACKGROUND
	HBITMAP hBitmap = nullptr;

	//读取同名bmp文件
	_tstring stBgPath = GetIniPath(_T(".bmp"));
	if (CStdFile::IfAccessFile(stBgPath))
	{
		hBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), stBgPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	else
	{
		hBitmap = ::LoadBitmap(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
	}

	//获取位图尺寸
	BITMAP bitmap;
	GetObject(hBitmap, sizeof (BITMAP), &bitmap);

	//获取对话框尺寸
	CRect rect;
	GetClientRect(&rect);

	//创建DC
	HDC m_hBkDC= ::CreateCompatibleDC(pDC->m_hDC);

	//绘图并清理
	if(hBitmap && m_hBkDC)
	{
		::SelectObject(m_hBkDC,hBitmap);
		::StretchBlt(pDC->m_hDC, 0, 0, rect.Width(), rect.Height(),m_hBkDC,0,0,bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		::DeleteObject(hBitmap);
		::DeleteDC(m_hBkDC);
	}
#endif // DLG_BACKGROUND

	//这个很重要
	return TRUE;
}

BOOL CDllTestorDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (MessageBox(_T("如果遇到任何问题需要帮助，\n请通过邮箱autumoon@vip.qq.com联系我!\n\n立即复制“autumoon@vip.qq.com”到剪贴板吗？"), 
		_T("需要帮助"), MB_YESNO) == IDYES)
	{
		//已经复制到剪贴板
		if(OpenClipboard())   
		{   
			_tstring source(_T("autumoon@vip.qq.com"));
			HGLOBAL clipbuffer;   
			char* buffer;   
			EmptyClipboard();   
			clipbuffer = GlobalAlloc(GMEM_DDESHARE,   source.size() + 1);   
			buffer = (char*)GlobalLock(clipbuffer);   
#ifdef _UNICODE
			std::string sSrc = CStdStr::ws2s(source);
#else
			std::string sSrc(source);
#endif // _UNICODE
			strcpy_s(buffer, source.size() + 1, sSrc.c_str());
			GlobalUnlock(clipbuffer);   
			SetClipboardData(CF_TEXT,clipbuffer);   
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
	int nItemCount = m_listItems.GetItemCount();
	for (int i = 0; i < nItemCount; ++i)
	{
		CString strCurItem = m_listItems.GetItemText(i, 0);
		_tstring stCurItem = CMfcStrFile::CString2string(strCurItem);
		vItems.push_back(stCurItem);
	}

	_tstring stDstDir = CMfcStrFile::CString2string(strDstDir);
	if (vItems.size() == 0 || !CStdDir::IfAccessDir(stDstDir) && !CStdDir::CreateDir(stDstDir))
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
	ppi->Start();
	CElapsedTime et;

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

#ifdef ITEM_ONLY_FILE
		if (CStdFile::IfAccessFile(stCurItem) && IsProperSuffix(stCurItem, m_cfg.vSuffixs))
		{
			ProcessFile(stCurItem, stDstItem, m_cfg);
		}
#endif // ITEM_ONLY_FILE

#ifdef ITEM_ONLY_DIR
		//如果是目录
		if(PathIsDirectory(stCurItem.c_str()))
		{
			//处理文件夹
			std::vector<_tstring> vSubFiles;
			size_t nSubFileCount = getFiles(stCurItem, vSubFiles, m_cfg.vSuffixs, true);

			for (int j = 0; j < nSubFileCount; ++j)
			{
				_tstring stSubSrcFile = vSubFiles[j];
				_tstring strSubDstFile = CStdStr::AddSlashIfNeeded(stDstItem) + stSubSrcFile.substr(CStdStr::AddSlashIfNeeded(stCurItem).length());
				_tstring strSubDir = CStdStr::GetDirOfFile(strSubDstFile);
				if (!CStdDir::IfAccessDir(strSubDir) && !CStdDir::CreateDir(strSubDir))
				{
					continue;
				}
				ProcessFile(stSubSrcFile, strSubDstFile, m_cfg);
			}
		}
#endif // ITEM_ONLY_DIR

		ppi->SetProgressValue(i + 1, nItemCount);
	}
	/*********************************这里增加主程序 结束***************************************/

	//结束耗时
	int nMin = 0, nSecond = 0, nMilliSecond = 0;
	et.End(nMin, nSecond, nMilliSecond);
	//结束日志
	//CLOG::Out(_T("%s"),_T("end task!"));
	//CLOG::Out(_T("This task costs %d min %d second %d millisecond!"), nMin, nSecond, nMilliSecond);
	//CLOG::End();

	//结束进度显示
	ppi->End();
	FlashWindow(TRUE);

#ifdef DLG_ELAPSED_TIME
	CString strTips;
	strTips.Format(_T("本次耗时 %d分%d秒%d毫秒!"), nMin, nSecond, nMilliSecond);
	AfxMessageBox(strTips);
#else
	AfxMessageBox(IDS_PROCESS_OVER);
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
		m_listItems.SetColumnWidth(0, max(10, rcList.Width() - 6));
	}

	if (nType != SIZE_MINIMIZED)
	{
		CRect rcDlg;
		GetClientRect(rcDlg);
		InvalidateRect(rcDlg);
		m_cfg.nWindowWidth = cx;
		m_cfg.nWindowHeight = cy;
	}
}
