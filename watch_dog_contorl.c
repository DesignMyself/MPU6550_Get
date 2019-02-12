#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/watchdog.h>

#define IWDG_DEVICE_NAME    "iwg"    /* 看门狗设备名称 */

 rt_device_t wdg_dev;         /* 看门狗设备句柄 */

static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
  
}
 int iwdg_sample()
{
    rt_err_t ret = RT_EOK;
    
    

    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(IWDG_DEVICE_NAME);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", IWDG_DEVICE_NAME  );
        return RT_ERROR;
    }
    /* 初始化设备 */
    ret = rt_device_init(wdg_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", IWDG_DEVICE_NAME  );
        return RT_ERROR;
    }
//    /* 设置看门狗溢出时间 */
//    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
//    if (ret != RT_EOK)
//    {
//        rt_kprintf("set %s timeout failed!\n", device_name);
//        return RT_ERROR;
//    }
    /* 设置空闲线程回调函数 */


    return ret;
}