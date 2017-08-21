// minimerge.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "uimerge.h"

#define USING_VC

typedef struct {

	char *name;
	char *data;
	int size;
} finfo;

int readfile(finfo *mfinfo) {

	int fd;
	struct stat stat_t;
	fd = open(mfinfo->name, O_RDONLY);
	if (fd < 0) {
		printf("Open %s failed \n", mfinfo->name);
		return 1;
	}

	if (fstat(fd, &stat_t) < 0) {
		printf("Get info about %s failed\n", mfinfo->name);
		close(fd);
		return 1;
	}

	mfinfo->data = (char *)malloc(stat_t.st_size);
	if (mfinfo->data == NULL) {
		printf("Allocating data for %s failed\n", mfinfo->name);
		close(fd);
		return 1;
	}
	mfinfo->size = stat_t.st_size;

	if (read(fd, mfinfo->data, mfinfo->size) < 0) {
		printf(" Read  %s failed \n", mfinfo->name);
		free(mfinfo->data);
		close(fd);
		return 1;
	}
	close(fd);

	return 0;
}
int writefile(finfo *mfinfo, char *buffer, int size) {

	int fd, rc;
	fd = open(mfinfo->name, O_RDWR | O_CREAT | O_TRUNC);
	if (fd < 0) {
		printf("Error opening %s\n", mfinfo->name);
		return 1;
	}

	rc = write(fd, &buffer[0], size);
	if (rc == 0) {
		printf("Error writing to %s\n", mfinfo->name);
		//free(buffer);
		return 1;
	}
	close(fd);
	return 0;

}

int cleanfile(finfo *mfinfo) {

	if (mfinfo->data != NULL) {

		free(mfinfo->data);
	}

	if (mfinfo->data != NULL)
		return 1;
	else
		return 0;
}

int merge(char *firstfile, char *secondfile,int pad, char *ofile) {

	finfo first;
	finfo second;
	finfo output;
	int size = 0, cur = 0;

	first.name = firstfile;
	second.name = secondfile;
	output.name = ofile;
	char *buffer;
	readfile(&first);
	readfile(&second);
	size = first.size + second.size;
	if (size > pad) {
		printf("pad error \n");
	}
	else{
		buffer = (char *)malloc(pad);
		if (buffer == NULL) {
			return 1;
		}
		memcpy(&buffer[cur], first.data, first.size);
		cur += first.size;
		memcpy(&buffer[cur], second.data, second.size);
		cur += second.size;
		memset(&buffer[cur], 0xff, pad-cur);
		if (writefile(&output, buffer, pad) != 0) {
			printf("writefile error \n");
			return  1;
		}
	}

	return 0;
}

int padding(char *srcfile, int addr) {
	finfo src;
	int cur = 0;

	src.name = srcfile;
	char *buffer;
	readfile(&src);
	if (src.size>addr) {
		printf("Padding address error\n");
		return 1;
	}
	buffer = (char *)malloc(addr);
	if (buffer == NULL) {
		return 1;
	}
	memcpy(&buffer[cur], src.data, src.size);
	cur += src.size;
	memset(&buffer[cur], 0xff, addr - src.size);
	if (writefile(&src, buffer, addr) != 0) {
		printf("writefile error \n");
		return 1;
	}
	return 0;
}

int htoi(char *s)
{
	int i = 0;
	int n = 0;
	int num = 0;
	int ret = 0;
	char *str = s;
	while (*str != '\0') {
		n++;
		str++;
	}
	for (i = 0; i<n; i++)
	{

		if ((s[i] <= '9') && (s[i] >= '0'))
			num = s[i] - '0';
		else if ((s[i] <= 'f') && (s[i] >= 'a'))
			num = s[i] - 'a' + 10;
		else if ((s[i] <= 'F') && (s[i] >= 'A'))
			num = s[i] - 'A' + 10;
		else
			break;
		ret = ret * 16 + num;
	}
	return ret;
}

void usage(void) {
	printf("example : minimerge -i image.bin -c csf.bin -p e000 -o merged_file.bin \n");
}


int main(int argc, char * argv[])
{
	char * image_file = NULL;
	char * csf_file = NULL;
	char * padaddr = NULL;
	char * out_file = NULL;
	int i, pad, ret;
	uimerge m_merge;
	for (i = 1; i < argc; i = i + 2) {
		if (!strcmp(argv[i], "-i")) {
			image_file = argv[i + 1];
		}
		else if (!strcmp(argv[i], "-c")) {
			csf_file = argv[i + 1];
		}
		else if (!strcmp(argv[i], "-p")) {
			padaddr = argv[i + 1];
		}
		else if (!strcmp(argv[i], "-o")) {
			out_file = argv[i + 1];
		}

	}
	if ((out_file == NULL) ||(image_file == NULL) || (csf_file == NULL)) {
		usage();
	}

	if (padaddr == NULL) {
		pad = 0;
	}
	else {
		pad = htoi(padaddr);
	}

#ifdef USING_VC
	CString image(image_file);
	CString csf(csf_file);
	CString output(out_file);
	ret=m_merge.merge(&image,&csf,&output,pad);
	if (ret == 0) {
		printf("Generate the merged file successfully\n");
	}
#else
	merge(image_file, csf_file, pad, out_file);
#endif
	return 0;
}
