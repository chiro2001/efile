#include "efiles.h"

int main() {
  const char *filename = "test.txt";
  printf("Efiles test module.  \n### SIMPLE TEST ###\n");
  efiles_init();
  EFILE *fp = efopen(filename, "w");
  if (!fp) {
    printf("Could not open %s to write!\n", filename);
    return 1;
  }
  char data[] = "Data to write!!";
  efwrite(data, sizeof(data), 1, fp);
  efclose(fp);
  printf("Write \"%s\" done!\n", data);

  EFILE *reader = efopen(filename, "r");
  if (!reader) {
    printf("Could not open %s to read!\n", filename);
    return 2;
  }
  char buf[512] = {0};
  efread(buf, sizeof(data), 1, fp);
  efclose(reader);
  printf("Read done! \"%s\"\n", buf);
  if (strcmp(buf, data) == 0)
    puts("Data is the same!");
  else {
    puts("Data differs!!");
    return 3;
  }
  puts("### SIMPLE TEST DONE ###");

  puts("### DELETE TEST ###");
  // 尝试删除文件
  EFILE *f_del = efopen(filename, "r");
  if (!f_del) {
    printf("Could not open %s to test!\n", filename);
    return 4;
  }
  int result = efiles_delete(f_del);
  if (result) {
    printf("Could not delete %s with code %d!\n", filename, result);
    return 5;
  }
  // 删除文件之后尝试打开
  EFILE *f_none = efopen(filename, "r");
  if (f_none) {
    printf("Opened a file does not exist...QAQ\n", filename);
    return 6;
  }
  puts("### DELETE TEST DONE ###");

  puts("### LARGE DATA TEST ###");
  EFILE *f_large = efopen(filename, "w");
  if (!f_large) {
    printf("Could not open %s to write!\n", filename);
    return 7;
  }
  // 测试1MB的数据
  char block[1024] = {0};
  int size_test_kb = 1024;
  int wrote = 0;
  for (int i = 1; i <= size_test_kb; i++) {
    wrote += efwrite(block, 1024, 1, f_large);
    printf("\rWrote %dB data!", wrote);
  }
  puts("\nWrote 1MB data done!");
#if 0
  // 测试9MB的数据
  size_test_kb = 1024 * 9;
  for (int i = 1; i <= size_test_kb; i++) {
    wrote += efwrite(block, 1024, 1, f_large);
    if (i % 100 == 0) printf("\rWrote %dB data!", wrote);
  }
  puts("\nWrote 9MB data done!");
  // 再测试写入一个字节
  puts("Try to wrote another byte...");
  wrote = efwrite(block, 1, 1, f_large);
  if (wrote == 0)
    puts("Writing to file failed!! Yeah!");
  else {
    puts("Writing to file successful...");
    puts("I must check the code...");
    printf("The file size: %d\n", f_large->size);
  }
#endif
  efclose(f_large);
  puts("### FORMAT I/O TEST ###");
  EFILE *f_format = efopen(filename, "w");
  if (!f_format) {
    printf("Could not open %s to test format!\n", filename);
    return 8;
  }
  // 格式化写入测试
  puts("Testing efprintf...");
  efprintf(f_format, "TEST INT: %d, FLOAT: %f, DOUBLE: %f, STRING: \"%s\"", 102,
           0.132, 3.141, "Test String...");
  efclose(f_format);
  EFILE *f_format_reader = efopen(filename, "r");
  if (!f_format_reader) {
    printf("Could not open %s to read format!\n", filename);
    return 9;
  }
  efread(buf, sizeof(buf), 1, f_format_reader);
  efclose(f_format_reader);
  printf("Got wrote string: %s\n", buf);
  puts("Testing efscanf...");
  f_format = efopen(filename, "r");
  if (!f_format) {
    printf("Could not open %s to test format!\n", filename);
    return 10;
  }
  int t1 = -1;
  float t2 = 1.0f;
  double t3 = 1.0;
  char t4[512] = "HEYHEYHEY";
  int count =
      efscanf(f_format, "TEST INT: %d, FLOAT: %f, DOUBLE: %lf, STRING: \"%s\"",
              &t1, &t2, &t3, t4);
  printf("Got %d data: INT: %d, FLOAT: %f, DOUBLE: %lf, STRING: \"%s\"\n",
         count, t1, t2, t3, t4);
  puts("### FORMAT I/O TEST DONE ###");
  return 0;
}