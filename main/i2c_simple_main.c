/* 

*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "OLED_128_64.h"

enum GAME_MOVE_TO{
    DOWN_AND_RIGHT,
    DOWN_AND_LEFT,
    UP_AND_RIGHT,
    UP_AND_LEFT
};


/**
 * 屏幕上一个点移动
 * 1个像素
 * 总是以45°移动
**/

void TaskShowGame(void *arg)
{
    static enum GAME_MOVE_TO move_to = DOWN_AND_RIGHT;  //初始向右下方移动
    static int pos_x, pos_y, last_x, last_y;            //显示的坐标
    pos_x = pos_y = last_x = last_y = 0;  //当前点的位置
    uint8_t ball = 0x01;//开始位置  屏幕左上角   0000 0001
    //每秒以45°移动  方向会在碰撞屏幕边缘改变  
    bool left = false;
    bool right = true;
    bool up = false;
    bool down = true;
    while (1)
    {
        
        OLED_ShowChar_simple(last_x, last_y, 0);    //清除当前位置
        OLED_ShowChar_simple(pos_x, pos_y, ball);
        last_x = pos_x;
        last_y = pos_y;         //记录当前位置 下次移动之前清除当前位置
        
        // printf("ball=%04xd\tpos_x=%d\tpos_y=%d\tleft=%d\tright=%d\tup=%d\tdown=%d\n", ball, pos_x, pos_y, left, right, up, down);
        /*开始移动位置*/
        //1.    往下移动的时候
        if(down)
        {
            if(pos_y == 7 && ball == 0x80)    //到达底部
            {
                down=false;
                up=true;
            }
            else if(ball == 0x80)
            {
                ball = 0x01;    //下一page
                pos_y++;
            }
        } 
        //2.    往上移动时
        if(up)
        {
            if(pos_y == 0 && ball == 0x01)  //到达顶部
            {
                up = false;
                down = true;
            }
            else if(ball == 0x01)
            {
                ball = 0x80;    //下一page
                pos_y--;
            }
        }
        //3.    往左移动时
        if(left)
        {
            if(pos_x == 0)
            {
                left = false;
                right = true;
            }
        }
        //4.    往右移动时
        if(right)
        {
            if(pos_x == 127)
            {
                right = false;
                left = true;
            }
        }

        if(right&&down)     move_to = DOWN_AND_RIGHT;
        else if(right&&up)  move_to = UP_AND_RIGHT;
        else if(left&&down) move_to = DOWN_AND_LEFT;
        else if(left&&up)   move_to = UP_AND_LEFT;

        switch (move_to)
        {
        case DOWN_AND_RIGHT:    //下右移动
            ball <<= 1; //⬇️
            pos_x++;    //➡️
            break;
        case DOWN_AND_LEFT:    //下左移动
            ball <<= 1; //⬇️
            pos_x--;    //⬅️
            break;
        case UP_AND_LEFT:    //上右移动
            ball >>= 1; //⬆️
            pos_x--;    //➡️
            break;
        case UP_AND_RIGHT:    //上右移动
            ball >>= 1; //⬆️
            pos_x++;    //⬅️
            break;
        
        }
        vTaskDelay( 10/portTICK_PERIOD_MS);     //移动速度
    }   
}




void app_main(void)
{
    
    OLED_128_64_init(); //初始化

    xTaskCreate(TaskShowGame, "ShowGame", 4096, NULL, 10, NULL);
    
}
