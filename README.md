# 基于Robomaster开发板C型与ADXL375三轴加速度传感器的四线式SPI通信说明手册

#目录
* [1.SPI通信原理(以ADXL375, 四线式为例)](#1spi通信原理以adxl375-四线式为例)
  * [CS](#cs)
  * [CLK](#clk)
  * [MOSI与MISO](#mosi与miso)
    * [MOSI时序](#mosi时序)
    * [MISO时序](#miso时序)
* [2.硬件连接与ioc文件配置](#2硬件连接与ioc文件配置)
    * [硬件连接](#硬件连接)
    * [ioc文件配置](#ioc文件配置)
    * [ioc配置解读](#ioc配置解读)
    * [为什么要使用软件来控制CS引脚?](#为什么要使用软件来控制cs引脚)
* [3.ADXL375.c程序解读](#3adxl375c程序解读adxl375h库文件解读略)
    * [`adxl_write (uint8_t address, uint8_t value)`](#adxl_write-uint8_t-address-uint8_t-value)
    * [`adxl_read (uint8_t address)`](#adxl_read-uint8_t-address)
    * [`adxl_init ()`](#adxl_init-)
    * [`adxl_Handle()`](#adxl_handle)
* [4.使用普源MSO5000测试](#4使用普源mso5000测试)
    * [何为"较为纯净"?](#何为较为纯净)
    * [正确时序](#正确时序)
    * [MSO5000的解码功能](#mso5000的解码功能仅作简单介绍)

## 1.SPI通信原理(以ADXL375, 四线式为例)

四线式SPI拥有四条信号线:CS CLK/SCL/SCK/SCLK MOSI/SDO MISO/SDI(注:本文档与RoboMaster开发板 C 型用户手册中使用CS CLK MOSI MISO命名方法, ADXL375使用手册中使用CS SCLK SDO SDI命名方法). ADXL375时序图如下:

![1.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/1.png)

### **CS**

CS为片选信号, 根据ADXL375使用手册(第15页-SPI模式), 当CS为高电平时开启I2C通信, 禁用SPI通信; 当CS为低电平时开启SPI通信, 禁用I2C通信.  

***注意: 如果主机使用SPI/I2C连接到多个外设(包括但不限于传感器)时, 若在对ADXL375的SPI通信停止时(即CS置于高电平), 总线上传输给其他外设的信号(MOSI信号)恰好对于ADXL375为有效的I2C信号, 则ADXL375将错误解读. 解决方法为添加一个PMOS, 只有当CS为低电平时使MOSI信号线导通. PMOS的阈值电压应符合ADXL375使用手册中规定的信号线电压值的范围(第17页-表11,12).***

### **CLK**

CLK为时钟信号.

### **MOSI与MISO**

MOSI(SPI Bus Master Output/Slave Input) SPI总线主机输出/从机输入  
MISO(SPI Bus Master Input/Slave Output) SPI总线主机输入/从机输出  

可见其已经规定了两根信号线信号发送的方向, 故不需要像RX/TX一样交叉连线(MISO-MISO, MOSI-MOSI, 但如果命名方式不是MISO/MOSI命名, 需要注意, 如ADXL375. 详见硬件连接部分).

#### **MOSI时序**

如时序图所示, 当MOSI(SDI)第一位(R/W位)为高电平(1)时, 进行读操作; 为低电平(0)时进行写操作. 若要读或写多个字节, 需要将第二位(MB位)置为高电平. 而后传输寄存器地址(ADDRESS BITS)与数据(DATA BITS).

当传感器接受到MOSI数据后会进行相应操作. 当第一位(R/W位)为低电平(0)时, 进行写操作: 对MISO中规定的寄存器地址(ADDRESS BITS)写入MOSI中规定的数据(DATA BITS)(如写入时序图的SDI时序).

#### **MISO时序**

当第一位(R/W位)为高电平(1)时进行读操作. 进行读操作时, 主机向传感器发送寄存器地址(ADDRESS BITS)后停用MOSI, 启用MISO(如读取时序图中大括号所标出的ADDRESS BITS和DATA BITS分别来自SDI(MOSI)与SDO(MISO)). 传感器会通过MISO线向主机传输MOSI中规定的地址(ADDRESS BITS)对应的寄存器的值.

## 2.硬件连接与ioc文件配置

### **硬件连接**

查阅"RoboMaster开发板 C 型用户手册"第10页(PDF第12页)有下图:

![2.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/2.png)

连接传感器供电与通信线(注意MOSI连接SDI, MISO连接SDO)

查阅"RoBoMaster开发板 C 型原理图"第3页有下图:

![3.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/3.png)

可见, SPI2四条信号线连接到STM32的引脚号, 下方ioc文件配置中设置的四个引脚号正来源于此.

### **ioc文件配置**

打开C板框架ioc文件(RM_Frame_C.ioc), 将PB12 PB13 PB14 PB15 PI2设置为Reset_State(这一步是因为前人配置错了, 帮他改过来). 打开左侧边栏的Connectivity-SPI2, 选择Mode为Full-Duplex Master(全双工主机), Hardware NSS Signal为Disable. Parameter Settings按如下配置(配置原因见解读部分):
|           选项名           |   选项    |
|  ------------------------  | -------- |
|        Frame Format        | Motorola |
|         Data Size          |  8 Bits  |
|          First Bit         | MSB First|
| Prescaler (for Baud Rate)  |    64    |
|   Clock Polarity (CPOL)    |   High   |
|     Clock Phase (CPHA)     |  2 Edge  |
|      CRC Calculation       | Disabled |
|      NSS Signal Type       | Software |

在右边栏将PB13 PB14 PB15分别设置为SPI2_SCK SPI2_MISO SPI2_MOSI, 将PB12设置为GPIO_Output. 点击Generate Code. 进入Src\main.c, 删除`main()`函数, 进入Src\stm32f4xx_it.c, 删除`USART3_IRQHandler()`函数.

### **ioc配置解读**

*Data Size*-8 Bits: 数据长度. ADXL375使用手册第21页:"ADXL375中的所有寄存器长度均为8位"

*First Bit*-MSB First: 左对齐(与程序中对应, 见程序解读部分)

*Prescaler (for Baud Rate)*-64: 总线分频值. ADXL375使用手册第21页:"100pF最大负载时的最大SPI时钟为5 MHz", 故需要设置分频器使Baud Rate小于5MBits/s. 对于C板所用的STM32F407, 分频器大于16时即可满足要求. 此处设置为64

*Clock Polarity (CPOL)*-High: ADXL375使用手册第21页:"时序方案要求时钟极性(CPOL) = 1"

*Clock Phase (CPHA)*-2 Edge: ADXL375使用手册第21页:"时钟相位(CPHA) = 1", CPHA=1在CubeMX中对应的配置为2 Edge

*CRC Calculation*-Disabled: 不需要CRC校验计算功能

*NSS Signal Type*-Software: 我们将使用`HAL_GPIO_Writepin()`函数来控制CS位, 故上方Hardware NSS Signal设为Disable, 此处设为Software

### **为什么要使用软件来控制CS引脚?**

Software NSS即使用简单的`HAL_GPIO_Writepin()`函数控制CS引脚, 因此可以作为主机连接多个外设(只需将要通信的外设的CS置低, 其他外设的CS置高即可).

Hardware NSS仍有许多bug(可以去各种论坛翻翻看), 并且一路SPI仅允许连接一个设备(因为Hardware NSS只能控制单个指定的CS引脚). 何况Software NSS使用起来非常方便, 因此在大多数场景下不建议使用Hardware NSS(RoboMaster开发板C型嵌入式软件教程文档中也用Software NSS来控制CS正是由于BMI088的陀螺仪与加速度计为两个独立的设备, 需要两个CS引脚进行控制).

## 3.ADXL375.c程序解读(ADXL375.h库文件解读略)

*例程中已将部分值改为宏定义, 可对照库文件查阅.*

### **`adxl_write (uint8_t address, uint8_t value)`**

为主机向ADXL375寄存器写入数据函数
`address`为要写入的寄存器地址, `value`为要写入的值.

```c
data[0] = address|0x40;
```

开启多字节传输.  

***注意: 即使读或写单字节仍需配置为多字节模式, 因为需要将寄存器地址+数据打包传输, 即如果写入数据为n字节, 共传输需要n+1个字节, 显然n+1>1.***  

根据时序图, 第一字节前两位为R/W位与MB位, 后六位(A5~A0)为地址位. 该行代码对address二进制下的每一位与0x40(二进制下01000000)取或运算(|), 即将address二进制下第二位置为1. 如address=0x32: 0x32在二进制下为00110010, 与0x40或运算后为01110010. 根据ADXL375使用手册第15页-SPI模式与第16页时序图, 设置位于第一个字节传输R/W位后的多字节位(MB位)为1则开启多字节传输.

```c
HAL_GPIO_WritePin (SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_RESET);
```

将CS置低, 开启SPI通信.

```c
HAL_SPI_Transmit (&hspi2, data, 2, 100);
```

传输数据. `&hspi2`代表SPI2(即用户SPI. C板已将SPI1分配给板载BMI088), `data`为要传输的数据, `2`为数据大小(2字节), `100`为超时, 单位为ms.

```c
HAL_GPIO_WritePin (SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_SET);
```

将CS置高, 停止SPI通信.

### **`adxl_read (uint8_t address)`**

为主机向ADXL375寄存器读取数据函数

```c
address |= 0x80;
address |= 0x40;
```

设置为多字节读取模式.
0x80二进制为10000000, 即将R/W位置高, 根据ADXL375使用手册第15页-SPI模式与第16页读取与写入时序图, R/W位置高为读取模式, 置低为写入模式. 0x40开启多字节传输.

![5.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/5.png)

```c
HAL_SPI_Transmit (&hspi2, &address, 1, 100);
```

写入一字节由上两行代码处理后的地址数据

```c
HAL_SPI_Receive (&hspi2, data_rec, 6, 100);
```

自传入的地址所指向的寄存器开始, 读取后六个寄存器(包含传入的地址所指向的寄存器)的数据, 存储到data_rec数组中. 以`address=0x32`为例, 将读取0x32~0x37(即三轴加速度数据)共六个寄存器, 存储到data_rec中.

![7.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/7.png)

### **`adxl_init ()`**

为ADXL375初始化函数

```c
adxl_write (0x31, 0x0f);
```

向0x31寄存器写入0x0f. 由ADXL375使用手册第24页-寄存器0x31—DATA_FORMAT(读/写):
无需自测, 则第一位为0; 为四线式SPI, 则第二位为0; 设置中断引脚高电平有效, 则第三位为0; MSB对齐模式, 则第六位为1, 其余位按已给规定设置. 则应向0x31写入00001111, 为十六进制0x0f.

```c
adxl_write (0x2d, 0x08);
```

设置ADXL375为测量模式. 由ADXL375使用手册第24 25页-寄存器0x2D—BW_RATE(读/写)
(注意:该手册中有两个0x2C寄存器的说明, 为该手册的错误. 第二个"0x2C"应为0x2D):
将测量位设置为1，置于测量模式, 则应向0x2D寄存器写入00001000, 为十六进制下0x08.

***注意: 对于其他配置, 如低功耗模式, 冲击检测, 传输速率等也应在此进行初始化. 读者不妨仿照上两行初始化代码尝试通过修改0x2C寄存器控制输出数据速率.(见ADXL375使用手册第10页表6与第15页-SPI模式对于输出数据速率的相关说明.***

### **`adxl_Handle()`**

为数据处理函数

```c
adxl_read (0x32);
```

读取0x32~0x37(即三轴加速度数据)共六个寄存器, 存储到缓存数组data_rec中.

```c
x = (int16_t)((data_rec[1]<<8)|data_rec[0]);
y = (int16_t)((data_rec[3]<<8)|data_rec[2]);
z = (int16_t)((data_rec[5]<<8)|data_rec[4]);
```

拼接x,y,z高八位与低八位数据并转为int16.

```c
xg = x*.049;
yg = y*.049;
zg = z*.049;
```

乘以比例因子, 将单位转化为g(重力加速度).

传感器输出数据以LSB表示, 需要乘以比例因子以转化为以重力加速度g为单位的加速度值. 此处比例因子0.049(灵敏度的倒数)为ADXL375使用手册第3页-技术规格中所示的49mg/LSB的典型值.

事实上, 比例因子并不固定. 不同的ADXL375在不同温度下比例因子不同(见ADXL375使用手册第8页图10~图15).

![8.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/8.png)

## 4.使用普源MSO5000测试

将普源MSO5000四个探头连接SPI四个引脚. 本文档按如下线序连接:
|      示波器通道       |SPI信号线|
|----------------------|--------|
|$\color{#FFD700}{CH1}$|   CS   |
|$\color{#33CCFF}{CH2}$|  CLK   |
|$\color{#FF00FF}{CH3}$|  MISO  |
|$\color{BLUE}{CH4}$   |  MOSI  |

按下Menu按钮, 设置触发类型为边沿触发, 信源为CS信号的通道, 边沿类型为下降沿(因为CS下降代表SPI开始通信).

呈现"较为纯净"的方波时, 则信号质量良好.

### **何为"较为纯净"?**

ADXL375使用手册规定了信号线电压值的范围(第17页-表11,12)

![4.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/4.png)

示波器所呈现的方波信号应符合该范围(其中数字输入为MOSI, 数字输出MISO).
其中$V_{DD I/O}=3.3V$.  
例: 数字输入低电平输入电压应小于$0.3\times V_{DD I/O}=0.99V$ 则示波器测量的低电平最大值应小于0.99V. 实际使用中要求并不严格, 可以适当放宽电压范围, 只要没有明显振荡即可.

### **正确时序**

对照ADXL375使用手册第16页时序图, 示波器显示的CS信号应有稳定的低电平平台, 时钟信号应有稳定的方波簇, MISO与MOSI应有频繁跳变(代表正在通信).

### **MSO5000的解码功能(仅作简单介绍)**

MSO5000内置了解码功能, 可以直接分析SPI信号.  
按下Decode, 选择解码1, 设置总线类型为SPI. 进入模式, 调整模式为片选, 片选信号为CS线对应的探头通道. 阈值设置为峰峰值的一半(1.5V). 按下Back以返回. 进入信源设置, 将CLK MISO MOSI均选为对应的通道, 阈值均为峰峰值的一半(1.5V). 返回并进入显示, 格式按偏好设置, 此处设为二进制. 返回并进入设置, 位序设为MSB, 宽度为8. 返回并打开总线开关, 可以看到示波器屏幕上显示出解码结果. 可以将程序与示波器结果对比. 如程序中`adxl_read (0x32);`将R/W位设为1, MB位设为1, 32二进制为00110010, 则MOSI第一字节应为11110010. 对照示波器解码结果查看, 结果正确, 如图:

![10.png](https://github.com/WangHaoZhe/SPI-Tutorial/blob/main/res/10.png)
