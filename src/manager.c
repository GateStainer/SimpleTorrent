#include "util.h"
#include "btdata.h"
#include <errno.h>

extern int errno;


int find_piece_len(int index,int j)
{
    
}

void sendBitField(int sockfd)
{
    
}

void *check_and_keepalive(void *p)
{

    

}

void sendRequest(int k)
{
    printf("k is %d\n", k);
    peer_t* my_peer = &peers_pool[k];
    int i, requestPiece = -1;
    pthread_mutex_lock(&least_prefer_mutex);
    if(least_prefer == 0)                   //非最小优先策略
    {
        for(i = 0; i < piecesNum; i ++)
        {
            pthread_mutex_lock(&my_peer->piecesInfo_mutex);
            if(piecesInfo[i] == 0 && my_peer->piecesInfo[i] == 1)
            {
                requestPiece = i;
                pthread_mutex_unlock(&my_peer->piecesInfo_mutex);
                break;
            }
            pthread_mutex_unlock(&my_peer->piecesInfo_mutex);
        }
    }
    else                                       //最小优先策略
    {
        int *have_piece_num = (int *)malloc(sizeof(int)*piecesNum);
        for(i=0;i<piecesNum;i++)
        {
            have_piece_num[i] = 0;
        }
        for(i = 0; i < MAXPEERS; i ++)
        {
            pthread_mutex_lock(&peers_pool[i].piecesInfo_mutex);
            if(peers_pool[i].used == 1 && peers_pool[i].status >= 2)
            {
                int j = 0;
                for(; j < piecesNum; j ++)
                {
                    if(peers_pool[i].piecesInfo[j] == 1)
                    {
                        have_piece_num[j] ++;
                    }
                }
            }
            pthread_mutex_unlock(&peers_pool[i].piecesInfo_mutex);
        }
        int min = MAXPEERS + 1;
        for(i = 0; i < piecesNum; i ++)
        {
            pthread_mutex_lock(&my_peer->piecesInfo_mutex);
            if(piecesInfo[i] == 0 && my_peer->piecesInfo[i] == 1)
            {
                if(have_piece_num[i] < min)
                {
                    min = have_piece_num[i];
                    requestPiece = i;
                }
            }
            pthread_mutex_unlock(&my_peer->piecesInfo_mutex);
        }
    }
    pthread_mutex_unlock(&least_prefer_mutex);
    printf("requestPiece is %d\n", requestPiece);
    if(requestPiece >= 0)
    {
        my_peer->isRequest = 1;
        pthread_mutex_lock(&g_mutex);
        piecesInfo[requestPiece] = 1;
        pthread_mutex_unlock(&g_mutex);
        int j;
        printf("subpiecesNum is %d\n", subpiecesNum[requestPiece]);
        for(j = 0; j < subpiecesNum[requestPiece]; j ++)
        {
            unsigned char *buffer = (char*)malloc(sizeof(int)*4 + sizeof(unsigned char));
            memset(buffer, 0, sizeof(int)*4 + sizeof(unsigned char));
            unsigned char *temp_buffer = buffer;

            int len = 13;
            len = htonl(len);
            memcpy(buffer, (char*)&len, sizeof(int));
            buffer += sizeof(int);
            len = ntohl(len);

            *buffer ++ = (unsigned char)6;

            int index = htonl(requestPiece);
            memcpy(buffer, (char*)&index, sizeof(int));
            buffer += sizeof(int);
            int begin = j * SUB_PIECE_LEN;
            begin = htonl(begin);
            memcpy(buffer, (char*)&begin, sizeof(int));
            buffer += sizeof(int);
            int len1 = find_piece_len(requestPiece,j);
            len1 = htonl(len1);
            memcpy(buffer, (char*)&len1, sizeof(int));
            printf("Now I will send Request pack to %s:%d\n", peers_pool[k].ip, peers_pool[k].port);
            printf("index is %d, begin is %d, len is %d\n",ntohl(index), ntohl(begin), ntohl(len1));
            printf("send to %d in sendRequest\n",my_peer->sockfd);
            int n = send(my_peer->sockfd, temp_buffer, sizeof(int)*4 + sizeof(char), 0);
            //printf("n is %d\n", n);
            free(temp_buffer);
        }
    }
    printf("now will return from sendRequest\n");
}


void sendRequestForEnd(int sockfd,int index)
{
    piecesInfo[index] = 1;
    int j;
    for(j=0; j<subpiecesNum[index]; j++)
    {
        unsigned char *buffer = (unsigned char *)malloc(sizeof(int)*4+sizeof(unsigned char));
        memset(buffer,0,sizeof(int)*4+sizeof(unsigned char));
        unsigned char *temp_buffer = buffer;
        int len = 13;
        len = htonl(len);
        memcpy(buffer,(char *)&len,sizeof(int));
        buffer += sizeof(int);
        len = ntohl(len);
        *buffer ++ = (unsigned char)6;
        index = htonl(index);
        memcpy(buffer, (char*)&index, sizeof(int));
        index = ntohl(index);
        buffer += sizeof(int);
        int begin = j * SUB_PIECE_LEN;
        begin = htonl(begin);
        memcpy(buffer, (char*)&begin, sizeof(int));
        buffer += sizeof(int);
        int len1 = find_piece_len(index,j);
        len1 = htonl(len1);
        memcpy(buffer, (char*)&len1, sizeof(int));
        //printf("Now I will send Request pack to %s:%d\n", peers_pool[k].ip, peers_pool[k].port);
        printf("index is %d, begin is %d, len is %d\n",ntohl(index), ntohl(begin), ntohl(len1));
        printf("send to %d in sendRequest\n",sockfd);
        int n = send(sockfd, temp_buffer, sizeof(int)*4 + sizeof(char), 0);
        //printf("n is %d\n", n);
        free(temp_buffer);
    }
}

void sendCancel(int sockfd,int index)
{
    int j;
    for(j=0; j<subpiecesNum[index]; j++)
    {
        unsigned char *buffer = (unsigned char *)malloc(sizeof(int)*4+sizeof(unsigned char));
        memset(buffer,0,sizeof(int)*4+sizeof(unsigned char));
        unsigned char *temp_buffer = buffer;
        int len = 13;
        len = htonl(len);
        memcpy(buffer,(char *)&len,sizeof(int));
        buffer += sizeof(int);
        len = ntohl(len);
        *buffer ++ = (unsigned char)8;
        index = htonl(index);
        memcpy(buffer, (char*)&index, sizeof(int));
        index = ntohl(index);
        buffer += sizeof(int);
        int begin = j * SUB_PIECE_LEN;
        begin = htonl(begin);
        memcpy(buffer, (char*)&begin, sizeof(int));
        buffer += sizeof(int);
        int len1 = find_piece_len(index,j);
        len1 = htonl(len1);
        memcpy(buffer, (char*)&len1, sizeof(int));
        //printf("Now I will send Request pack to %s:%d\n", peers_pool[k].ip, peers_pool[k].port);
        //printf("index is %d, begin is %d, len is %d\n",ntohl(index), ntohl(begin), ntohl(len1));
        // printf("send to %d in sendRequest\n",my_peer->sockfd);
        int n = send(sockfd, temp_buffer, sizeof(int)*4 + sizeof(char), 0);
        //printf("n is %d\n", n);
        free(temp_buffer);
    }
}

void sendPiece(int sockfd, int index, int begin, int len)
{
    unsigned char *send_buff = (unsigned char *)malloc(sizeof(int)*3+sizeof(unsigned char)*(1+len));
    memset(send_buff,0,sizeof(int)*3+sizeof(unsigned char)*(1+len));
    unsigned char* temp_buff = send_buff;

    int send_len = sizeof(int) * 2 + sizeof(char) * (1 + len);
    int send_len_n = htonl(send_len);
    printf("piece pack_len is %d,",send_len);
    memcpy(send_buff, (char*)&send_len_n, 4);
    assert(*(int *)send_buff == send_len_n);
    send_buff += 4;
    *send_buff ++ = 7;

    int index_n = htonl(index);
    printf("index is %d,",index);
    memcpy(send_buff, (char*)&index_n, 4);
    send_buff += 4;

    int begin_n = htonl(begin);
    printf("begin is %d\n",begin);
    memcpy(send_buff, (char*)&begin_n, 4);
    send_buff += 4;

    file2buffer(index, begin, len, send_buff);
    printf("Now I will send piece pack\n");
    int n = send(sockfd, temp_buff, sizeof(int) * 3 + sizeof(unsigned char) * (1 + len), 0);
    assert(sizeof(int)*3 + sizeof(unsigned char)*(1+len) == n);
    g_uploaded += len;
    free(temp_buff);
}

void sendHave(int sockfd, int index)
{
    unsigned char *send_buff = (unsigned char *)malloc(sizeof(int)*2 + 1);
    memset(send_buff,0,sizeof(int)*2+1);
    unsigned char* temp_buff = send_buff;

    int send_len = sizeof(int) + sizeof(char);
    int send_len_n = htonl(send_len);
    memcpy(send_buff, (char*)&send_len_n, 4);
    send_buff += 4;
    *send_buff ++ = 4;

    int index_n = htonl(index);
    strncpy(send_buff, (char*)&index_n, 4);
    printf("Now I will send have pack\n");
    send(sockfd, temp_buff, sizeof(int) * 2 + sizeof(unsigned char), 0);
    free(temp_buff);
}
void sendInterested(int sockfd)
{
    unsigned char *send_buff = (unsigned char *)malloc(sizeof(int)+1);
    memset(send_buff,0,sizeof(int)+1);
    unsigned char *temp_buffer = send_buff;

    int send_len = 1;
    send_len = htonl(send_len);
    memcpy(send_buff,(char *)&send_len,4);
    send_buff +=4;
    *send_buff ++ = 2;
    printf("now I will send interested pack\n");
    send(sockfd,temp_buffer,sizeof(int)+sizeof(unsigned char),0);
    free(temp_buffer);
}
void sendUnchoked(int sockfd)
{
    unsigned char *send_buff = (unsigned char *)malloc(sizeof(int)+1);
    memset(send_buff,0,sizeof(int)+1);
    unsigned char *temp_buffer = send_buff;

    int send_len = 1;
    send_len = htonl(send_len);
    memcpy(send_buff,(char *)&send_len,4);
    send_buff +=4;
    *send_buff ++ = 1;
    printf("now I will send unchoked pack\n");
    send(sockfd,temp_buffer,sizeof(int)+sizeof(unsigned char),0);
    free(temp_buffer);
}
