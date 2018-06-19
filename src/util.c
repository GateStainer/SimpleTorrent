
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "util.h"


/* End recvline */

// recvlinef(int fd, char *format, ...)
// ����: ���׽���fd�����ַ���.�������������ָ��һ����ʽ�ַ���, ��������洢��ָ���ı�����
// ����: �׽���������fd, ��ʽ�ַ���format, ָ�����ڴ洢������ݵı�����ָ��
// ���: ��ȡ���ֽ���
int recvlinef(int fd, char *format, ...)
{
    va_list argv;
    va_start(argv, format);

    int retVal = -1;
    char *line;
    int lineSize = recvline(fd, &line);

    if (lineSize > 0)
    {
        retVal = vsscanf(line, format, argv);
        free(line);
    }

    va_end(argv);

    return retVal;
}
/* End recvlinef */

int reverse_byte_orderi(int i)
{
    unsigned char c1, c2, c3, c4;
    c1 = i & 0xFF;
    c2 = (i >> 8) & 0xFF;
    c3 = (i >> 16) & 0xFF;
    c4 = (i >> 24) & 0xFF;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}
