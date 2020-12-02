## EFiles

用C语言模拟一个在内存中操作文件的API。

### API

```C
// 上层 API
EFILE *efopen(const char *filename, const char *mode);
size_t efread(void *buf, size_t size, size_t count, EFILE *stream);
size_t efwrite(void *buf, size_t size, size_t count, EFILE *stream);
size_t eftell(EFILE *stream);
int efclose(EFILE* stream);
int efscanf(EFILE *stream, const char *format, ...);
int efprintf(EFILE *stream, const char *format, ...);
// 中层 API
void efiles_write(EFILE *stream, void *data, size_t size);
void efiles_read(EFILE *stream, void *data, size_t size);
void efiles_init();
// 内部函数
EFILE *efiles_find(const char *filename);
EFILE *efiles_create(const char *filename);
int efiles_delete(EFILE *stream);
int efiles_close(EFILE *stream);
void efiles_clear();
void efiles_mark(EFILE *steam, EFILE_FLAG flag);
int efiles_readable(EFILE *stream);
int efiles_writeable(EFILE *stream);
```

### 配置

```C
// efiles.h line 19
// 最大文件名长度
#define EFILE_FILENAME_LEN 512

//efiles.c line 15
// 最大文件大小：默认10MB
int efiles_max_files_size = 1024 * 1024 * 10;
// 最多文件个数：默认1024
int efiles_max_file_num = 1024;
```

