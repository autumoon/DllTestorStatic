// DllTestorDlg.h : 头文件
//

#pragma once
#include <afxcmn.h>
#include <set>
#include "Configure.h"		//配置文件
#include "ElapsedTime.h"	//输出耗时
#include "GetDirFiles.h"	//获取目录文件
#include "CLog.h"			//输出日志
#include "MfcStrFile.h"		//mfc字符串等
#include "ProgressInterface.h"
#include "MyEdit.h"			//支持拖拽
#include "EasySize.h"		//支持缩放

#include "BuildConfig.h"		//编译配置

// CDllTestorDlg 对话框
class CDllTestorDlg : public CDialogEx
{
	DECLARE_EASYSIZE
// 构造
public:
	CDllTestorDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CDllTestorDlg();

	enum class ProcessMode
	{
		File,
		Dir,
		Both
	};

// 对话框数据
	enum { IDD = IDD_DLLTESTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	HBITMAP m_hBgBitmap;

	ProcessMode m_eProcessMode;

	//大小写不敏感集合用于去重
	struct CaseInsensitiveLess
	{
		bool operator()(const _tstring& a, const _tstring& b) const
		{
			return _tcsicmp(a.c_str(), b.c_str()) < 0;
		}
	};
	std::set<_tstring, CaseInsensitiveLess> m_itemSet;

	void RebuildItemSet();

	//配置文件参数
	config_s m_cfg;
	//获取配置文件路径
	_tstring GetIniPath(const TCHAR* szFileExt = _T(".ini"));

	//判断后缀是否符合条件
	bool IsProperSuffix(const _tstring& strFilePath, const std::vector<_tstring>& vSuffix);

	//添加到列表
	int AddItemToList(_tstring stItemPath);

	//处理单个文件
	bool ProcessFile(const _tstring& stSrcPath, const _tstring& stDstPath, config_s& _cfg);
	
#ifdef CMD_OUTPUT
	int SetCommandLine();
	int ReleaseCommandLine();
#endif

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_listItems;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonAddItems();
	afx_msg void OnBnClickedButtonClearItems();
	afx_msg void OnBnClickedButtonDelItems();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	//主要实现
	CMyEdit m_eDstDir;
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
