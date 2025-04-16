#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2kern/io/fcntl.h>
#include <psp2kern/io/stat.h>
#include <taihen.h>
#include <stdarg.h>
#include <string.h>

#include "logPlus.h"

static SceUID log_thid = -1;  // 线程句柄
static SceUID log_mutex = -1; // 互斥锁句柄
static SceUID log_fd = -1;    // 文件句柄
static bool enabled = true;   // 是否启用日志线程

static unsigned int log_buffer_ptr = 0;
static int flag = 0;
static char log_buffer[16 * 1024];

// ✍️ 全局可调用：格式化并写入日志文件
void log_write(const char *buffer, size_t length)
{
    if (!enabled)
    {
        return;
    }

#ifndef RELEASE
    // 如果当前buffer有值的部分长度+新内容的长度，超过buffer的总长度，则返回
    if ((log_buffer_ptr + length) >= sizeof(log_buffer))
        return;

    // 拷贝内存数据块
    memcpy(log_buffer + log_buffer_ptr, // dest：目标地址（把数据复制到这里）, char[] + unsigned = char * 获取log_buf的最后一位的地址，
           buffer,                      // src：源地址（从这里读取数据）
           length);                     // n：要复制的字节数

    log_buffer_ptr = log_buffer_ptr + length;
#endif
}

// 🧵 日志线程：保持打开文件，可用于定时 flush 等操作（此例中只初始化）
int log_thread(SceSize args, void *argp)
{
    ksceIoMkdir(LOG_PATH, 6); // 创建路径

    // 获取文件句柄，只写、不存在则创建，追加内容
    SceUID fd = ksceIoOpen(LOG_FILE,
                           SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 6);
    if (fd < 0)
        return 0;

    while (1)
    {
        ksceIoWrite(fd, log_buffer, strlen(log_buffer));
        memset(log_buffer, 0, sizeof(log_buffer));
        log_buffer_ptr = 0;
        // log_printf("日志线程启动\n");
        //  休眠 10 秒（1000 毫秒 = 1000000 微秒）
        ksceKernelDelayThread(10 * 1000 * 1000);
    }

    ksceIoClose(fd);
    return 0;
}

void log_start()
{
    if (!enabled)
    {
        return;
    }

    // 打开日志文件（不存在就创建），追加模式
    log_fd = ksceIoOpen(LOG_FILE,
                        SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC,
                        0666);
    if (log_fd < 0)
    { // 文件句柄获取失败
        return;
    }
    ksceIoClose(log_fd);
    // 创建互斥锁，避免并发写入
    log_mutex = ksceKernelCreateMutex("log_mutex", 0, 0, NULL);

    if (log_mutex < 0)
    {
        ksceIoClose(log_fd);
        return;
    }

    // 启动日志线程（可用于定时处理，也可以不用）
    log_thid = ksceKernelCreateThread("log_thread", log_thread, 0x10000100, 0x1000, 0, 0, NULL);

    if (log_thid >= 0)
    {
        ksceKernelStartThread(log_thid, 0, NULL);
    }

    // log_write("模块启动完成\n");
}