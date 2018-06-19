#include "btdata.h"
#include "util.h"

extern int g_done;

void client_shutdown(int sig)
{
  g_done = 0;
}
