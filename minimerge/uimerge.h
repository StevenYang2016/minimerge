#pragma once
#include "afx.h"
typedef struct {
	CString *fname;
	char *data;
	int   size;
} finfo_t;
class uimerge
{
public:
	uimerge();
	~uimerge();
	int read_file(finfo_t *finfo);
	int merge(CString  *strImageFile, CString *strCsfFile, CString  *OutPutFile, int padaddr);
	int CStringToHex(CString num);
};

