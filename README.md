# TimerKey
## 跨平台的按键检测库，基于定时器实现，简单易用
# 特性
- 支持检测短按、长按、双击、连续按下等状态
- 由定时器驱动，仅需50Hz低频就可以完成工作
- 采用面向对象的思想进行封装
- 硬件无关，跨平台
- 无需第三方依赖
- C语言设计

# 移植
- **TimerKey**的代码为C语言设计，支持C++环境，移植无需其他步骤，直接将源代码添加到项目中即可
- 对象的创建默认使用C语言的内存标准库，如有特殊的内存分配需求，可以将头文件中`tkey_malloc`和`tkey_free`宏替换为项目中所使用的内存分配函数

# 使用
## STEP 1
创建一个按键对象并初始化对象，可以选择创建默认按键对象或者自定义按键对象
### 创建默认按键对象
调用`tkey_create_default`函数创建默认按键对象  
默认按键对象的属性如下:  
- 按键扫描处理函数的工作频率:`50Hz`
- 去抖时间:`1 tick` (20ms@50Hz)
- 长按时间:`25 tick` (500ms@50Hz)
- 连续按下时间间隔:`15 tick` (300ms@50Hz)
- 按键按下时电平:`0` (低电平)

### 创建自定义按键对象
调用`tkey_create`函数创建自定义按键对象，使用`tkey_config_t`结构体初始化按键对象  
结构体参数如下:  
- `event_cb`:事件回调函数
- `detect_cb`:检测回调函数
- `user_data`:传入回调函数的用户数据
- `hold_ticks`:长按持续时间
- `debounce_ticks`:去抖时间
- `multi_press_interval_ticks`:连续按下间隔时间
- `pressed_level`:按键按下时电平

## STEP 2
初始化完成后再周期性地调用`tkey_handler`函数处理按键的扫描事件,每个按键对象都需要调用该函数进行处理  
或者周期性地调用`tkey_mutli_handler`函数处理多个相同配置的按键对象
# 回调函数编写
## 检测回调函数
```c
int tkey_detect_cb(void *user_data)
{
    int pin = (int)user_data;
    return gpio_read(pin);
}
```
可以在注册回调函数时通过`user_data`传入需要检测的引脚，这样可以将这个检测回调函数注册在不同的按键对象中
## 事件回调函数
所有的按键事件如下:  
- `TKEY_EVENT_PRESS`:按键按下时的事件
- `TKEY_EVENT_LONG_PRESS`:判定为长按时的事件
- `TKEY_EVENT_MULTI_PRESS`:判定为多次按下时的事件
- `TKEY_EVENT_RELEASE`:按键释放时的事件
- `TKEY_EVENT_LONG_RELEASE`:按键长按后释放时的事件
- `TKEY_EVENT_MULTI_RELEASE`:按键多次按下后释放时的事件
- `TKEY_EVENT_DEFAULT_PRESS`:默认的按键按下事件，包括按下和多次按下
- `TKEY_EVENT_ALL_PRESS`:所有按下的事件，包括按下、长按和多次按下
- `TKEY_EVENT_ALL_RELEASE`:所有释放的事件，包括释放、长按后释放、多次按下后释放

不同的事件可以通过`|`与操作来完成多状态检测
```c
void tkey_event_cb(tkey_handle_t key, tkey_event_t event, uint8_t press_count, void *user_data)
{
    switch(key)
    {
        case key1:
        if(event & TKEY_EVENT_DEFAULT_PRESS)
        {
            printf("key1 pressed!\r\n");
            printf("key1 pressed count:%d\r\n", (int)press_count);
            ...
        }
        break;
        case key2:
        if(event & (TKEY_EVENT_RELEASE | TKEY_EVENT_LONG_RELEASE))
        {
            printf("key2 released!\r\n");
            ...
        }
        break;
        ...
    }
}
```
事件回调函数的参数如下:  
- `key`:按键对象
- `event`:事件
- `press_count`:按下次数
- `user_data`:用户数据