//author:autumoon
//mail:autumoon@vip.qq.com
#include "Configure.h"

_tstring VectorToString(const std::vector<_tstring>& vStrings)
{
	_tstring strRes;

	const size_t num = vStrings.size();

	if (num > 0)
	{
		for (size_t i = 0; i < num - 1; ++i)
		{
			const _tstring& strCurItems = vStrings[i];
			strRes += strCurItems + _T('|');
		}

		strRes += vStrings[vStrings.size() - 1];
	}

	return strRes;
}

static std::vector<_tstring> FilterEmptyStrings(const std::vector<_tstring>& vStrings)
{
	std::vector<_tstring> vRes;
	for (size_t i = 0; i < vStrings.size(); ++i)
	{
		if (vStrings[i].length() > 0)
		{
			vRes.push_back(vStrings[i]);
		}
	}

	return vRes;
}

int ReadIniFile(const _tstring& strIniPath, config_s& _cfg)
{
	bool bRes = CStdFile::IfAccessFile(strIniPath.c_str());

	CSimpleIni Ini;
	Ini.SetUnicode();
	if (bRes)
	{
		if (Ini.LoadFile(strIniPath.c_str()) < 0)
		{
			Ini.Reset();
			return 1;
		}

		//读取信息
		_cfg.bRemPath = Ini.GetBoolValue(INI_PRESUFFIX, INI_REMPATH, _cfg.bRemPath);

		_tstring strDstDirs = VectorToString(_cfg.vDstPaths);
		strDstDirs = Ini.GetValue(INI_PRESUFFIX, INI_DST_DIRS, strDstDirs.c_str());
		_cfg.vDstPaths = FilterEmptyStrings(CStdStr::Split(strDstDirs, _T("|")));

		_tstring strAllItems = VectorToString(_cfg.vItemPaths);
		strAllItems = Ini.GetValue(INI_PRESUFFIX, INI_ALL_ITEMS, strAllItems.c_str());
		_cfg.vItemPaths = FilterEmptyStrings(CStdStr::Split(strAllItems, _T("|")));

		_tstring strSuffixs = VectorToString(_cfg.vSuffixs);
		strSuffixs = Ini.GetValue(INI_PRESUFFIX, INI_FILE_SUFFIXS, strSuffixs.c_str());
		_cfg.vSuffixs = FilterEmptyStrings(CStdStr::Split(strSuffixs, _T("|")));
		if (_cfg.vSuffixs.size() == 0)
		{
			_cfg.vSuffixs.push_back(_T("*"));
		}

		_cfg.nWindowWidth = Ini.GetLongValue(INI_PRESUFFIX, INI_WIN_WIDTH, _cfg.nWindowWidth);
		_cfg.nWindowHeight = Ini.GetLongValue(INI_PRESUFFIX, INI_WIN_HEIGHT, _cfg.nWindowHeight);
	}
	else
	{
		WriteIniFile(strIniPath, _cfg);
	}
	Ini.Reset();

	return 0;
}

int WriteIniFile(const _tstring& strIniPath, const config_s& _cfg)
{
	CSimpleIni Ini;
	Ini.SetUnicode();

	Ini.SetBoolValue(INI_PRESUFFIX, INI_REMPATH, _cfg.bRemPath);
	Ini.SetValue(INI_PRESUFFIX, INI_DST_DIRS, VectorToString(_cfg.vDstPaths).c_str());
	Ini.SetValue(INI_PRESUFFIX, INI_ALL_ITEMS, VectorToString(_cfg.vItemPaths).c_str());
	Ini.SetValue(INI_PRESUFFIX, INI_FILE_SUFFIXS, VectorToString(_cfg.vSuffixs).c_str());
	Ini.SetLongValue(INI_PRESUFFIX, INI_WIN_WIDTH, _cfg.nWindowWidth);
	Ini.SetLongValue(INI_PRESUFFIX, INI_WIN_HEIGHT, _cfg.nWindowHeight);

	if (Ini.SaveFile(strIniPath.c_str()) < 0)
	{
		Ini.Reset();
		return 1;
	}

	Ini.Reset();

	return 0;
}
