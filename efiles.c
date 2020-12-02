/*
- Description: 模拟文件系统。提供模拟的文件读写功能。需要提前写入相关数据。
- Version: 0.0.1
- Author: Chiro
- Email: Chiro2001@163.com
*/

#include "efiles.h"

// 文件列表：一个元素为 FILE* 类型的数组，指向所有的文件
EFILE **root = NULL;
// 文件列表尾部指针，指向下一个的位置
EFILE **tail = NULL;
// 最大文件大小：默认10MB
int efiles_max_files_size = 1024 * 1024 * 10;
// 最多文件个数：默认1024
int efiles_max_file_num = 1024;
// 是efprintf等要用到的内存
char _efiles_buf_[EFILE_BUF_SIZE];

/* ========== 内部功能 ========== */

// @Description: 清除文件列表
void efiles_clear() {
  if (!root) return;
  EFILE **p = root;
  while (p) free(*p++);
  free(root);
}

// @Description: 查找文件
// @Return: EFILE* / NULL
EFILE *efiles_find(const char *filename) {
  // 直接顺序查找文件名
  // TODO: 使用堆优化（但是我懒得做
  for (EFILE **p = root; *p; p++) {
    if (strcmp((*p)->filename, filename) == 0) return *p;
  }
  return NULL;
}

// @Description: 新建一个文件
// @Return: EFILE*
EFILE *efiles_create(const char *filename) {
  if (efiles_find(filename) != NULL) {
    return NULL;
  }
  if (tail - root >= efiles_max_file_num) return NULL;
  EFILE *stream = (EFILE *)malloc(sizeof(EFILE));
  memset(stream, 0, sizeof(EFILE));  // 设置到默认的写入状态
  strcpy(stream->filename, filename);
  *tail = stream;
  tail++;
  return stream;
}

// @Description: 关闭目前正在读取的文件(也就是重置一下offset等状态)
//               请记得把对应指针置NULL
int efiles_close(EFILE *stream) {
  stream->flag = EFILE_FLAG_WRITE;
  stream->mode_flag = EFILE_MODE_WRITE;
  stream->offset = 0;
  return 0;
}

// @Description: 删除一个文件
// @Retrun: 0 -- 成功; 非零 -- 失败
int efiles_delete(EFILE *stream) {
  if (!stream) return 1;
  // 先释放对应数据区
  if (!stream->data) return 2;
  free(stream->data);
  // 然后从文件列表删除这个指针
  EFILE **target = NULL;
  for (EFILE **p = root; *p; p++)
    if (stream == *p) {
      target = p;
      break;
    }
  if (!target) return 3;
  // EFILE *t = *target;
  *target = *(tail - 1);
  // *(tail - 1) = t;
  *(tail - 1) = NULL;
  tail--;

  // 最后删除这个指针指向的内存
  free(stream);
  return 0;
}

// @Description: 标记一个文件的当前状态
void efiles_mark(EFILE *stream, EFILE_FLAG flag) {
  if (!stream) return;
  stream->flag = flag;
}

// @Description: 该文件是否可读
int efiles_readable(EFILE *stream) {
  if (!stream) return 0;
  if (stream->flag & EFILE_FLAG_ERROR || !stream->flag & EFILE_FLAG_READ)
    return 0;
  return 1;
}

// @Description: 该文件是否可写
int efiles_writeable(EFILE *stream) {
  if (!stream) return 0;
  if (stream->flag & EFILE_FLAG_ERROR || stream->flag & EFILE_FLAG_READ)
    return 0;
  return 1;
}

/* ========== 中层 API ========== */

// @Description: 初始化函数：初始化文件列表信息
void efiles_init() {
  root = (EFILE **)malloc(sizeof(EFILE *) * efiles_max_file_num);
  memset(root, 0, sizeof(EFILE *) * efiles_max_file_num);
  tail = root;
}

// @Description: 对文件写入数据
void efiles_write(EFILE *stream, void *data, size_t size) {
  // 申请内存空间
  void *temp = realloc(stream->data, size + stream->size);
  if (!temp) {
    efiles_mark(stream, EFILE_FLAG_WRITE_ERROR);
    return;
  }
  stream->data = temp;
  // 可能不对
  memcpy((uint8_t *)stream->data + stream->offset, data, size);
  stream->offset += size;
  stream->size += size;
}

// @Description: 读取文件数据
size_t efiles_read(EFILE *stream, void *data, size_t size) {
  if (!stream || !data || !size) return;
  size = size + stream->offset > stream->size ? stream->size - stream->offset
                                              : size;
  memcpy(data, (uint8_t *)stream->data + stream->offset, size);
  stream->offset += size;
  return size;
}

/* ========== 上层 API ========== */

EFILE *efopen(const char *filename, const char *mode) {
  // 暂时只支持两个操作："w" / "r"
  if (!filename || !mode || !*filename || !*mode) return NULL;
  EFILE *exist = efiles_find(filename);
  // 写入
  if (*mode == 'w') {
    // 对已经存在的文件进行覆盖，也就是先删除
    if (exist) {
      efiles_delete(exist);
    }
    EFILE *file = efiles_create(filename);
    return file;
  } else if (*mode == 'r') {
    // 读取
    if (!exist) return NULL;
    exist->flag = EFILE_FLAG_READ;
    return exist;
  }
  // 不支持的操作
  return NULL;
}

size_t eftell(EFILE *stream) {
  if (!stream) return 0;
}

size_t efwrite(void *buf, size_t size, size_t count, EFILE *stream) {
  if (!buf || !size || !count || !stream || stream->flag & EFILE_FLAG_ERROR)
    return 0;
  uint8_t *p = buf;
  for (size_t i = 0; i < count; i++) {
    efiles_write(stream, p, size);
    if (stream->size >= efiles_max_files_size) {
      return (p - (uint8_t *)buf) * size;
    }
    p += size;
  }
  return size * count;
}

size_t efread(void *buf, size_t size, size_t count, EFILE *stream) {
  if (!buf || !size || !count || !stream || stream->flag & 0x80) return 0;
  uint8_t *p = buf;
  size_t wrote = 0;
  for (size_t i = 0; i < count; i++) {
    wrote = efiles_read(stream, p, size);
    if (wrote != size) return p - (uint8_t *)buf;
    p += size;
  }
  return size * count;
}

int efclose(EFILE *stream) { return efiles_close(stream); }

// 使用宏定义完成
// int efscanf(EFILE *stream, const char *format, ...);
// int efprintf(EFILE *stream, const char *format, ...);
