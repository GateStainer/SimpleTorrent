
#include "bencode.h"
#include "util.h"
#include "sha1.h"


char* find_nodes(char *data,int len)
{

    int i = 0;
    for(; i<len-strlen("e5:nodes"); i++)
    {
        if(strncmp(data+i,"e5:nodes",strlen("e5:nodes")) == 0)
        {
            return data+i;
        }
    }
    return NULL;
}


// 注意: 这个函数只能处理单文件模式torrent
torrentmetadata_t* parsetorrentfile(char* filename)
{
    
}
