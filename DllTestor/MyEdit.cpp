// MyEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "DllTestor.h"
#include "MyEdit.h"

#include <vector>


// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit():m_nFlag(EDIT_NONE_JUDGE)
{
	
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMyEdit 消息处理程序

void CMyEdit::PreSubclassWindow()
{
	CEdit::PreSubclassWindow();
	DragAcceptFiles(TRUE);
}

void CMyEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(hDropInfo)
	{
		//拖拽文件的数量
		UINT nDrag = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

		//只拖拽一个文件
		if (nDrag > 1 && (m_nFlag & EDIT_SIG_JUDGE))
		{
			nDrag = 1;
		}

		// 被拖拽的文件的文件名
		std::vector<TCHAR> szItemPath(MAX_PATH, 0);

		CString strAllPath;
		for (UINT i = 0; i < nDrag; ++i)
		{
			DragQueryFile(hDropInfo, i, szItemPath.data(), (UINT)szItemPath.size());
			if (PathFileExists(szItemPath.data()) == TRUE)
			{
				bool bAdd = false;
				//根据类型判断
				if ((m_nFlag & (EDIT_DIR_JUDGE | EDIT_FILE_JUDGE)) == 0)
				{
					bAdd = true;
				}
				else if ((m_nFlag & EDIT_DIR_JUDGE) && PathIsDirectory(szItemPath.data()))
				{
					bAdd = true;
				}
				else if ((m_nFlag & EDIT_FILE_JUDGE) && !PathIsDirectory(szItemPath.data()))
				{
					bAdd = true;
				}

				if (bAdd)
				{
					//多个项目时，前面已经有内容才添加间隔符
					if (strAllPath.GetLength() > 0)
					{
						strAllPath += _T(";");
					}
					strAllPath += szItemPath.data();
				}
			}
		}

		SetWindowText(strAllPath);
	}
	DragFinish(hDropInfo);
}
