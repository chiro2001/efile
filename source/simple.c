#include <stdio.h>
#include "efiles.h"

int main() {
  efiles_init();
  EFILE *f = efopen("test.txt", "w");
  if (!f) return 1;
  efprintf(f, "%dÊÇÊý¾Ý", 100);
  efclose(f);
  f = efopen("test.txt", "r");
  if (!f) return 1;
  char buf[512] = {0};
  char data;
  efscanf(f, "%d%s", &data, buf);
  printf("got data: data = %d, buf = %s\n", data, buf);
  return 0;
}