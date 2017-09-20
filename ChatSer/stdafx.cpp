
// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// ChatSer.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

int strsplit(CString &src, CString &c, CString *dst, int max)
{
	int cnt = 0;
	int index = src.Find(c);

	//CString data = src;
	//CString str;

	while (index != -1 && cnt < max) {
		dst[cnt++] = src.Mid(0, index);
		src = src.Mid(index + c.GetLength(), src.GetLength() - index - c.GetLength());
		// str.Format(_T("%s(%d)\n%s"), data, index, src);
		// MessageBox(NULL, str, _T(""), 0);
		index = src.Find(c);
	}

	if (cnt < max) {
		dst[cnt++] = src;
	}
	return cnt;
}