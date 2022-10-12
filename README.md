# [ESP-Sensory](https://github.com/stops-top/ESP-Sensory)

[![Build Status](https://github.com/stops-top/ESP-Sensory/workflows/ci/badge.svg)](https://github.com/stops-top/ESP-Sensory/actions/workflows/ci.yml)

用于实现末端固件烧录和监控管理，实现更灵活的网络测试部署

基于BOX开发的触屏交互逻辑，后续完善更多联网功能，用于边缘端设备管理和监控：

* 视频和语音的远程监控-视频门铃方案

PMOD的只要接口资源依赖：

* UART1+UART2
* SDMMC 
* USB

## TODO

* 完善烧录和调试功能，实现任务体分离
* 在烧录工具的基础上实现日志管理和传输功能
* 在日志管理基础上完善本地的交互显示和设置


基于 ESP-NOW的无线调试功能和日志功能，完成测试的本地存储和数据导出