#include "stdafx.h"
#include "uimerge.h"


uimerge::uimerge()
{
}


uimerge::~uimerge()
{
}

int uimerge::read_file(finfo_t * finfo)
{
	CFile file;
	int data_size;

	if (file.Open(*(finfo->fname), CFile::modeReadWrite) == 0) {
		printf("Error opening %S\n", *(finfo->fname));
		return 1;
	}
	data_size = file.GetLength();

	finfo->data = (char *)malloc(data_size);
	if (finfo->data == NULL) {
		printf("Error allocating data for %S\n", *(finfo->fname));
		file.Close();
		return 1;
	}

	finfo->size = data_size;

	if (file.Read(finfo->data, finfo->size) < 0) {
		printf("Error reading %S\n", *(finfo->fname));
		free(finfo->data);
		file.Close();
		return 1;
	}
	file.Close();
	return 0;
}

int uimerge::merge(CString * strImageFile, CString * strCsfFile, CString * OutPutFile, int padaddr)
{
	finfo_t csf_bin;
	finfo_t initimage_bin;
	int size, rc, data_cur = 0;
	char *buffer;
	CFile file;
	csf_bin.fname = strCsfFile;
	initimage_bin.fname = strImageFile;
	if (read_file(&csf_bin)) {
		return 1;
	}
	if (read_file(&initimage_bin)) {
		return 1;
	}
	size = csf_bin.size + initimage_bin.size;
	if (padaddr == 0) {
		buffer = (char*)malloc(size);
		if (buffer == NULL)
			return(1);
		memcpy(&buffer[data_cur], initimage_bin.data, initimage_bin.size);
		data_cur += initimage_bin.size;
		memcpy(&buffer[data_cur],csf_bin.data, csf_bin.size);
		data_cur += csf_bin.size;
		if (file.Open(*OutPutFile, CFile::modeCreate | CFile::modeReadWrite) == 0) {
			printf("Error opening %S\n", *OutPutFile);
			free(buffer);
			return 1;
		}
		file.Write(&buffer[0], data_cur);
		rc = file.GetLength();
		if (rc == 0) {
			printf("Error writing to %S\n", *OutPutFile);
			free(buffer);
			return 1;
		}
		file.Close();
		free(buffer);
	}
	else {
		if (padaddr <= size) {
			return 1;
		}
		buffer = (char*)malloc(padaddr);
		if (buffer == NULL) {
			return(1);
		}
		memcpy(&buffer[data_cur], initimage_bin.data, initimage_bin.size);
		data_cur += initimage_bin.size;
		memcpy(&buffer[data_cur], csf_bin.data, csf_bin.size);
		data_cur += csf_bin.size;
		memset(&buffer[data_cur], 0xff, padaddr - data_cur);
		if (file.Open(*OutPutFile, CFile::modeCreate | CFile::modeReadWrite) == 0) {
			printf("Error opening %S\n", *OutPutFile);
			free(buffer);
			return 1;
		}
		file.Write(&buffer[0], padaddr);
		rc = file.GetLength();
		if (rc == 0) {
			printf("Error writing to %S\n", *OutPutFile);
			free(buffer);
			return 1;
		}
		file.Close();
		free(buffer);

	}

	return 0;
}

int uimerge::CStringToHex(CString num)
{
	int i = 0;
	int n = 0;
	int len = 0, ret = 0;
	len = num.GetLength();
	for (i = 0; i<len; i++)
	{

		if ((num[i] <= '9') && (num[i] >= '0'))
			n = num[i] - '0';
		else if ((num[i] <= 'f') && (num[i] >= 'a'))
			n = num[i] - 'a' + 10;
		else if ((num[i] <= 'F') && (num[i] >= 'A'))
			n = num[i] - 'A' + 10;
		else
			break;
		ret = ret * 16 + n;
	}
	return ret;
}
