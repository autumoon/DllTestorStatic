#pragma once

//author:autumoon
//2018.01.18
//xcopy /d /y .\src\*.dll $(TargetDir)

#define INI_PRESUFFIX		_T("INI_PRESUFFIX")
#define INI_REMPATH			_T("INI_REMPATH")
#define INI_ALL_ITEMS		_T("INI_ALL_ITEMS")
#define INI_DST_DIRS		_T("INI_DST_DIRS")
#define INI_FILE_SUFFIXS	_T("INI_FILE_SUFFIXS")
#define INI_WIN_WIDTH		_T("INI_WIN_WIDTH")
#define INI_WIN_HEIGHT		_T("INI_WIN_HEIGHT")

#ifdef _UNICODE

#ifndef _tstring
#define _tstring std::wstring
#endif // _tstring

#else

#ifndef _tstring
#define _tstring std::string
#endif // _tstring

#endif
