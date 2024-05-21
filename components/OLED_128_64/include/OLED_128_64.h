#ifndef __OLED_128_64_H__
#define __OLED_128_64_H__


#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
//I2C端口号
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define OLED_CMD 0
#define OLED_DATA 1
#define OLED_I2C_ADDR 0x78
/**
 * @author  liu
 * @date    2024-05-18
 * @details OLED     i2c初始化
 *
**/
void OLED_128_64_init(void);
void OLED_ShowString(int x, int y, char * Show_char);   //显示字符串
void OLED_ShowChar_simple(int x, int y, uint8_t data);       //显示字符
#endif