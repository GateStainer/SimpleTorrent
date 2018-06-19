#include "util.h"
#include "btdata.h"

announce_url_t* parse_announce_url(char* announce)
{
  char* announce_ind;
  char port_str[6];  // �˿ں����Ϊ5λ����
  int port_len = 0; // �˿ں��е��ַ���
  int port;
  char* url;
  int url_len = 0;
  announce_ind = strstr(announce,"/announce");
  printf("announce_ind is %s\n",announce_ind);
  announce_ind--;
  while(*announce_ind != ':')
  {
    port_len++;
    announce_ind--;
  }
  strncpy(port_str,announce_ind+1,port_len);
  port_str[port_len] = '\0';
  port = atoi(port_str);
  printf("222\n"); 
  char* p;
  for(p=announce; p<announce_ind; p++)
  {
    url_len++;   
  }

  announce_url_t* ret;
  ret = (announce_url_t*)malloc(sizeof(announce_url_t));
  memset(ret,0,sizeof(announce_url_t));
  if(ret == NULL)
  {
    perror("Could not allocate announce_url_t");
    exit(-73);
  }
 
  p = announce;
  printf("ANNOUNCE: %s\n",announce);
  if(strstr(announce,"http://") > 0)
  {
    url_len -= 7;
    p += 7;
  }
  
  url = (char*)malloc((url_len+1)*sizeof(char)); // +1 for \0
  strncpy(url,p,url_len);
  url[url_len] = '\0';
  
  ret->hostname = url;
  ret->port = port;

  return ret;
}