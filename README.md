# AWTK 针对 STM32H743 的移植。

* [AWTK](https://github.com/zlgopen/awtk) 全称 Toolkit AnyWhere，是 [ZLG](http://www.zlg.cn/) 开发的开源 GUI 引擎，旨在为嵌入式系统、WEB、各种小程序、手机和 PC 打造的通用 GUI 引擎，为用户提供一个功能强大、高效可靠、简单易用、可轻松做出炫酷效果的 GUI 引擎。

[awtk-stm32h743iitx-freertos](https://github.com/zlgopen/awtk-stm32h743iitx-freertos) 是 AWTK 在 STM32H743 上的移植。

本项目以 [正点原子阿波罗 STM32H743 开发板 STM32H7](https://item.taobao.com/item.htm?id=571115692251&ali_refid=a3_430582_1006:1103191143:N:HIGgPtZ%2BAhQpeEvXe%2FK1RspPjJRVCvOC:5b41a06d9f58d787690cd30a8865e887&ali_trackid=1_5b41a06d9f58d787690cd30a8865e887&spm=a230r.1.14.8#detail) 为载体移植，其它开发板可能要做些修改，有问题请请创建 issue。

本项目除了实现基本功能的移植外，还提供了如下功能：

* 集成实时操作系统 (RTOS)（FreeRTOS)
* 集成 FATFS 文件系统，访问 SD 卡的数据。
* 实现从文件系统加载应用程序的资源。
* 使用 Sqlite 存储数据。
* 支持 google 拼音输入法。

## 编译

1. 获取源码

> 将几个仓库取到同一个目录下：

```
git clone https://github.com/zlgopen/awtk.git
git clone https://github.com/zlgopen/awtk-demo-app.git
git clone https://github.com/zlgopen/awtk-sqlite3.git
git clone https://github.com/zlgopen/awtk-fs-adapter.git
git clone https://github.com/zlgopen/awtk-stm32h743iitx-freertos.git
```

2. 用 keil 打开 USER/awtk.uvprojx

3. 将资源放到 T 卡的目录/awtk/assets/中。

4. 如果不想将资源放到 T 卡，可在 awtk_config.h 中注释掉下面的代码：
```
#define WITH_FS_RES 1
```
