#include <drivers/rtc.h>
#include <fs/fs.h>
#include <mm/alloc.h>
#include <panic.h>
#include <math.h>
#include <string.h>
#include <dbg.h>

static uint8_t bcd2Bin(uint8_t bcd)
{
        return (bcd & 0x0F) + ((bcd >> 4) * 10);
}

static uint8_t rtcRead(uint8_t reg)
{
        outb(RTC_ADDRESS_PORT, reg);
        return inb(RTC_DATA_PORT);
}

rtcTime_t rtcGetTime(void)
{
        rtcTime_t time = {0};
        time.second = bcd2Bin(rtcRead(RTC_SECONDS));
        time.minute = bcd2Bin(rtcRead(RTC_MINUTES));
        time.hour = bcd2Bin(rtcRead(RTC_HOURS));
        time.day = bcd2Bin(rtcRead(RTC_DAY));
        time.month = bcd2Bin(rtcRead(RTC_MONTH));
        time.year = bcd2Bin(rtcRead(RTC_YEAR));
        return time;
}

static int read(file_t *file, void *buf, size_t i)
{
        int a = min(i, sizeof(rtcTime_t));
        not_optional(file);
        not_optional(buf);
        rtcTime_t time = rtcGetTime();
        memcpy(buf, &time, a);
        return a;
}

static file_ops_t fil = {
        .read = read,
};

void rtcInit(void)
{
        vnode_t *node = vfs_create("/dev", "rtc", 0);
        node->ops = &fil;
        LOG(" [rtc] initialised at %x\r\n", node);
}
