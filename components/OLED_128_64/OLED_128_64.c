#include <stdio.h>
#include "OLED_128_64.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "oledfont.h"


// 向OLED写入指令字节
//----------------------------------------------------------------------------
int OLED_Write_Command(int OLED_Byte)
{
    // static uint8_t buffer[512] = {0};
    //创建I2C命令链  静态创建要提供buffer 相比 i2c_cmd_link_create动态自动分配可以避免内存碎片化问题
    // i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    if(!handle)
    {
        ESP_LOGE("OLED", "i2c_cmd_link_create_static() fail!");
        return false;
    }

	i2c_master_start(handle);					// 发送起始信号
    i2c_master_write_byte(handle, OLED_I2C_ADDR, true);
    i2c_master_write_byte(handle, 0x00, true);  //表示要写入的是命令
    i2c_master_write_byte(handle, OLED_Byte, true);
    i2c_master_stop(handle);

    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_MASTER_NUM, handle, portMAX_DELAY)); //100/portTICK_PERIOD_MS

    i2c_cmd_link_delete(handle);
    
	return true;						// 返回写入成功
}
//----------------------------------------------------------------------------

// 向OLED写入数据字节
//----------------------------------------------------------------------------
int OLED_Write_Data(int OLED_Byte)
{
    // static uint8_t buffer[512] = {0};
    //创建I2C命令链  静态创建要提供buffer 相比 i2c_cmd_link_create动态自动分配可以避免内存碎片化问题
    // i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    if(!handle)
    {
        ESP_LOGE("OLED", "i2c_cmd_link_create_static() fail!");
        return false;
    }

	i2c_master_start(handle);					// 发送起始信号
    i2c_master_write_byte(handle, OLED_I2C_ADDR, true);
    i2c_master_write_byte(handle, 0x40, true);  //表示要写入的是数据
    i2c_master_write_byte(handle, OLED_Byte, true);
    i2c_master_stop(handle);

    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_MASTER_NUM, handle, portMAX_DELAY));

    i2c_cmd_link_delete(handle);
    
	return true;						// 返回写入成功
}
//----------------------------------------------------------------------------

// 向OLED写入一字节数据/指令
//---------------------------------------------------------------
void  OLED_WR_Byte(int OLED_Byte, int OLED_Type)
{
	if(OLED_Type)
		OLED_Write_Data(OLED_Byte);		// 写入数据
	else
		OLED_Write_Command(OLED_Byte);	// 写入指令
}
//---------------------------------------------------------------

// 整屏写入某值
//------------------------------------------------------------------------
void  OLED_Clear(void)
{
	int N_Page, N_row;

	for(N_Page=0; N_Page<8; N_Page++)
	{
		OLED_WR_Byte(0xb0+N_Page,OLED_CMD);	// 从0～7页依次写入
		OLED_WR_Byte(0x00,OLED_CMD);      	// 列低地址
		OLED_WR_Byte(0x10,OLED_CMD);      	// 列高地址

		for(N_row=0; N_row<128; N_row++)OLED_WR_Byte(0x00,OLED_DATA);
	}
}
//------------------------------------------------------------------------

// 设置数据写入的起始行、列
//------------------------------------------------------------------------
void  OLED_Set_Pos(int x, int y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);				// 写入页地址

	OLED_WR_Byte((x&0x0f),OLED_CMD);  			// 写入列的地址(低半字节)

	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);	// 写入列的地址(高半字节)
}
/**
 * @brief i2c master initialization
 * @liu   主机模式初始化
 */
static esp_err_t i2c_master_init(void)
{
    //i2c初始化
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,            //配置主机模式
        .sda_io_num = I2C_MASTER_SDA_IO,    //SDA SCL引脚
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,    //是否启用 ESP32 的内部上拉电阻
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ, //时钟速度
    };
    //配置驱动模式
    i2c_param_config(i2c_master_port, &conf);
    //安装驱动程序
    /*参数3（仅限从机模式）分配用于在从机模式下发送和接收数据的缓存区大小。
     *I2C 是一个以主机为中心的总线，数据只能根据主机的请求从从机传输到主机。
     *因此，从机通常有一个发送缓存区，供从应用程序写入数据使用。数据保留在发送缓存区中，由主机自行读取
    */
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


void OLED_128_64_init(void)
{
    ESP_ERROR_CHECK(i2c_master_init()); //初始化i2c
    ESP_LOGI("OLED init", "I2C initialized successfully");

	OLED_WR_Byte(0xAE,OLED_CMD);	// 关闭显示

	OLED_WR_Byte(0x00,OLED_CMD);	// 设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);	// 设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);	// 设置起始行地址
	OLED_WR_Byte(0xB0,OLED_CMD);	// 设置页地址

	OLED_WR_Byte(0x81,OLED_CMD); 	// 对比度设置，可设置亮度
	OLED_WR_Byte(0xFF,OLED_CMD);	// 265

	OLED_WR_Byte(0xA1,OLED_CMD);	// 设置段(SEG)的起始映射地址
	OLED_WR_Byte(0xA6,OLED_CMD);	// 正常显示；0xa7逆显示

	OLED_WR_Byte(0xA8,OLED_CMD);	// 设置驱动路数（16~64）
	OLED_WR_Byte(0x3F,OLED_CMD);	// 64duty

	OLED_WR_Byte(0xC8,OLED_CMD);	// 重映射模式，COM[N-1]~COM0扫描

	OLED_WR_Byte(0xD3,OLED_CMD);	// 设置显示偏移
	OLED_WR_Byte(0x00,OLED_CMD);	// 无偏移

	OLED_WR_Byte(0xD5,OLED_CMD);	// 设置震荡器分频
	OLED_WR_Byte(0x80,OLED_CMD);	// 使用默认值

	OLED_WR_Byte(0xD9,OLED_CMD);	// 设置 Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);	// 使用官方推荐值

	OLED_WR_Byte(0xDA,OLED_CMD);	// 设置 com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);	// 使用默认值

	OLED_WR_Byte(0xDB,OLED_CMD);	// 设置 Vcomh，可调节亮度（默认）
	OLED_WR_Byte(0x40,OLED_CMD);	// 使用官方推荐值

	OLED_WR_Byte(0x8D,OLED_CMD);	// 设置OLED电荷泵
	OLED_WR_Byte(0x14,OLED_CMD);	// 开显示

	OLED_WR_Byte(0xAF,OLED_CMD);	// 开启OLED面板显示

	OLED_Clear();        			// 清屏

	OLED_Set_Pos(0,0); 				// 设置数据写入的起始行、列
}
// 在指定坐标处显示一个字符
//-----------------------------------------------------------------------------
void OLED_ShowChar(int x, int y, char Show_char)
{
	char c=0,i=0;

	c = Show_char - ' '; 				// 获取字符的偏移量

	if(x>128-1){x=0;y=y+2;}	// 当列数超出范围，则另起2页

	// if(SIZE ==16) 					// 字符大小为[8*16]：一个字符分两页
	{
		// 画第一页
		//-------------------------------------------------------
		OLED_Set_Pos(x,y);						// 设置画点起始处
		for(i=0;i<8;i++)  						// 循环8次(8列)
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); 	// 找到字模

		// 画第二页
		//-------------------------------------------------------
		OLED_Set_Pos(x,y+1); 					// 页数加1
		for(i=0;i<8;i++)  						// 循环8次
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);// 把第二页画完
	}
}
// 在指定坐标起始处显示字符串
//-------------------------------------------------------------------
void  OLED_ShowString(int x, int y, char * Show_char)
{
	int N_Char = 0;		// 字符序号

	while (Show_char[N_Char]!='\0') 	// 如果不是最后一个字符
	{
		OLED_ShowChar(x,y,Show_char[N_Char]); 	// 显示一个字符

		x += 8;					// 列数加8，一个字符占8列

		if(x>=128){x=0;y+=2;} 	// 当x>=128，另起一页

		N_Char++; 				// 指向下一个字符
	}
}
//-------------------------------------------------------------------
// 清屏
//------------------------------------------------------------------------

//在指定位置显示 一个字节
void OLED_ShowChar_simple(int x, int y, uint8_t data)
{
    OLED_Set_Pos(x, y);
    OLED_WR_Byte(data, OLED_DATA);
}