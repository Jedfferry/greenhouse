# 温室大棚系统

### 开发时间：2014年05月--2015年05月
### 开发环境：`IAR` `Visual Studio2012`
### 开发语言：`C` `C#`
### 使用器件：OURS-IOV2-cc2530模块、
### 获奖情况：
   本项目为“**国家级大学生创新训练项目**”，并代表学校参加全国大学生创新创业**年会**展示(**全校共2项**）。
### 项目背景：
本项目搭建了大棚系统物联网，对大棚内**温度**、**湿度**和**光照强度**等信息实时监控，并通过协调器将数据通过串口上传给PC服务器进行分析和存储，PC客户端程序则实现实时数据显示、管理员对大棚内环境的控制（如利用灯光控制、通风等操作保证种植物所需的基本条件）。
### 开发人员
  
* 莫云明

* 何坤炎

* 俸毅

### 本人负责的内容

1.独立完成数据采集功能的实现，利用RS232协议将各类传感器连接到Zigbee终端上，并用ZStack协议栈搭建物联网，终端采集各类数据（每2.5s）后通过Zigbee技术无线传输给协调器，协调器通过串口上传数据给上位机；<br>
2.完成环境控制模块的开发，使用C51单片机驱动LED灯、电风扇等元器件工作，实现大棚内光照、通风等控制。<br>

