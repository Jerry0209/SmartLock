/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <drv_lcd.h>
#include <rttlogo.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

#include <rtdbg.h>

#define PIN_KEY0        GET_PIN(A, 0)     // PC0:  KEY0         --> KEY
#define PIN_KEY1        GET_PIN(A, 1)      // PC1 :  KEY1         --> KEY
#define PIN_KEY2        GET_PIN(A, 2)      // PC4 :  KEY2         --> KEY
#define PIN_KEY3        GET_PIN(A, 3)     // PC0:  KEY0         --> KEY
#define PIN_KEY4        GET_PIN(B, 6)      // PC1 :  KEY1         --> KEY
#define PIN_KEY5        GET_PIN(B, 7)      // PC4 :  KEY2         --> KEY
#define PIN_KEY6        GET_PIN(B, 14)     // PC0:  KEY0         --> KEY
#define PIN_BEEP        GET_PIN(B, 0)      // PA1:  BEEP         --> BEEP (PB1)



volatile int Bflag = 0;
volatile int flag = 0;
volatile int kflag = 0;
volatile int UnlockFlag = 0;
volatile int eflag = 0;
volatile int lockflag = 0;

volatile int Sub = 0;
volatile int errornum = 0;


int stored[4]={2,5,8,0};
int temp[4]={0};

rt_thread_t thread;
            /* 创建thread1线程 */


void init(void)
{
    rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);/* 设置蜂鸣器引脚为输出模式 */

    lcd_clear(WHITE); /* show RT-Thread logo */
    lcd_show_image(0, 0, 240, 69, image_rttlogo);/* set the background color and foreground color */
    lcd_set_color(WHITE, BLACK);
    lcd_show_string(65, 110, 32, "_ _ _ _");

    Bflag = 0;
    flag = 0;
    kflag = 0;
    UnlockFlag = 0;
    eflag = 0;
    lockflag = 0;

    Sub = 0;
    errornum = 0;



}

int ROW_READ(void)
        {
            rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
            rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
            rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
            rt_pin_mode(PIN_KEY3, PIN_MODE_INPUT_PULLUP);
            rt_pin_mode(PIN_KEY4, PIN_MODE_OUTPUT);
            rt_pin_mode(PIN_KEY5, PIN_MODE_OUTPUT);
            rt_pin_mode(PIN_KEY6, PIN_MODE_OUTPUT);

            rt_pin_write(PIN_KEY4, PIN_LOW);
            rt_pin_write(PIN_KEY5, PIN_LOW);
            rt_pin_write(PIN_KEY6, PIN_LOW);


            if(rt_pin_read(PIN_KEY0) == PIN_LOW)
            {
                return 1;

            }

            if(rt_pin_read(PIN_KEY1) == PIN_LOW)
            {
                return 2;

            }

            if(rt_pin_read(PIN_KEY2) == PIN_LOW)
            {
                return 3;

            }

            if(rt_pin_read(PIN_KEY3) == PIN_LOW)
            {
                return 4;

            }

            else
            {
                return 0;
            }
        }



int COLUMN_READ(void)
        {
            rt_pin_mode(PIN_KEY0, PIN_MODE_OUTPUT);
            rt_pin_mode(PIN_KEY1, PIN_MODE_OUTPUT);
            rt_pin_mode(PIN_KEY2, PIN_MODE_OUTPUT);
            rt_pin_mode(PIN_KEY3, PIN_MODE_OUTPUT);
            rt_pin_mode(PIN_KEY4, PIN_MODE_INPUT_PULLUP);
            rt_pin_mode(PIN_KEY5, PIN_MODE_INPUT_PULLUP);
            rt_pin_mode(PIN_KEY6, PIN_MODE_INPUT_PULLUP);

            rt_pin_write(PIN_KEY0, PIN_LOW);
            rt_pin_write(PIN_KEY1, PIN_LOW);
            rt_pin_write(PIN_KEY2, PIN_LOW);
            rt_pin_write(PIN_KEY3, PIN_LOW);


            if(rt_pin_read(PIN_KEY4) == PIN_LOW)
            {
                return 1;

            }

            if(rt_pin_read(PIN_KEY5) == PIN_LOW)
            {
                return 2;

            }

            if(rt_pin_read(PIN_KEY6) == PIN_LOW)
            {
                return 3;

            }

            else
            {
                return 0;
            }
        }


int Sum(void)
{   int sum =0;
    if(ROW_READ() != 0 && COLUMN_READ() != 0 && ROW_READ() != 4)
    {sum = COLUMN_READ()+3*(ROW_READ()-1);}

    else if (ROW_READ() == 4 && COLUMN_READ() == 2)
    {
        sum = 0;
    }
    else if (ROW_READ() == 4 && COLUMN_READ() == 1)
    {
        sum = 10;
    }
    else if (ROW_READ() == 4 && COLUMN_READ() == 3)
    {
        sum = 11;
    }
    return sum; //得出按键值
}

void Read_Key(void)
{
    if (ROW_READ()==1)
    {
        //lcd_show_num(10, 200, 1, 1, 32);
        Bflag=1;

    }

    else if(ROW_READ()==2)
    {
        //lcd_show_num(10, 200, 2, 1, 32);
        Bflag = 1;

    }

    else if(ROW_READ()==3)
    {
        //lcd_show_num(10, 200, 3, 1, 32);
        Bflag = 1;


    }
    else if(ROW_READ()==4)
    {
        //lcd_show_num(10, 200, 4, 1, 32);
        Bflag = 1;


    }

    //if(ROW_READ()==0)
    else
    {

        Bflag = 0;
        flag = 0;
        kflag = 0;
    }

    //ROW
}

void Input_Key(void)
{
    if(Bflag == 1 && UnlockFlag == 0)
    {

        if(Sub<4)
        {
            if (Sum() != 11 && Sum() != 10 && kflag == 0)
            {
                Sub++;//
                temp[Sub-1] = Sum();//
                lcd_show_string(65+32*(Sub-1), 100, 32, "*");
                kflag = 1;

            }
        }
    }
}

void Unlock(void)
{
    if(Sum() == 11 && UnlockFlag ==0)//按下确认键
    {
        if(Bflag ==1 && kflag==0)
        {
            if(Sub==4)
            {

                if(temp[0] == stored[0] && temp[1] == stored[1] && temp[2] == stored[2] && temp[3] == stored[3] && Sub==4)//
                {

                    lcd_clear(WHITE); /* show RT-Thread logo */
                    lcd_show_image(0, 0, 240, 69, image_rttlogo);
                    lcd_set_color(WHITE, BLACK);
                    lcd_show_string(65, 110, 32, "Welcome!");
                    UnlockFlag = 1;

                    for (int i = 0; i < 4; ++i)
                    {
                        temp[i]=0;

                    }

                    rt_thread_mdelay(5000);

                    init();



                }

                else
                {
                    errornum++;

                    if (errornum < 5)
                    {
                        lcd_clear(WHITE); /* show RT-Thread logo */
                        lcd_show_image(0, 0, 240, 69, image_rttlogo);
                        lcd_set_color(WHITE, BLACK);
                        lcd_show_string(50, 110, 32, "Incorrect");

                        rt_thread_mdelay(1000);

                        lcd_clear(WHITE); /* show RT-Thread logo */
                        lcd_show_image(0, 0, 240, 69, image_rttlogo);
                        lcd_set_color(WHITE, BLACK);
                        lcd_show_string(65, 110, 32, "_ _ _ _");

                        Sub = 0;

                        for (int i = 0; i < 4; ++i)
                        {
                            temp[i]=0;
                        }

                        kflag = 0;
                    }

                    else if (errornum >= 5)
                    {
                        lcd_clear(WHITE); /* show RT-Thread logo */
                        lcd_show_image(0, 0, 240, 69, image_rttlogo);
                        lcd_set_color(WHITE, BLACK);
                        lcd_show_string(65, 110, 32, "Locked!");
                        lockflag = 1;

                    }

                }
            }
        }

     }
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
static struct rt_thread thread2;

static void Beep(void)
{
    while(1)
    {
        if(Bflag == 1 && flag == 0)
        {
            rt_pin_write(PIN_BEEP,PIN_HIGH);
            //Delay(250);
            rt_thread_mdelay(100);//Beep
            rt_pin_write(PIN_BEEP,PIN_LOW);
            flag = 1;
        }

        rt_thread_mdelay(10);
    }
}

void thread_entry(void *parameter)
{

    while (1)
    {
        if (lockflag == 1)
        {
            rt_thread_mdelay(10000);
            init();
        }

    }
}





int main(void)
{

       init();

       rt_thread_init(&thread2,
                      "thread2",
                      Beep,
                      RT_NULL,
                      &thread2_stack[0],
                      sizeof(thread2_stack),
                      0, THREAD_TIMESLICE);

        rt_thread_startup(&thread2);//Beep

        thread = rt_thread_create("thread1", thread_entry, (void *)10,
                          THREAD_STACK_SIZE,
                          THREAD_PRIORITY, THREAD_TIMESLICE);

        rt_thread_startup(thread);



        while(1)
        {
//            ROW_READ();
//            rt_hw_us_delay(10000);//按键消抖
            //rt_thread_mdelay(10);
//            COLUMN_READ();
//            rt_hw_us_delay(10000);
            //rt_thread_mdelay(10);
            //Sum();
            //lcd_show_num(20, 120, Sum(), 1, 32);

            Read_Key();
            Input_Key();
            Unlock();

            rt_thread_mdelay(100);
        }


    return 0;
}




///* 定时器的控制块 */
//static rt_timer_t timer1;
//
//static int cnt = 0;
///* 定时器1超时函数 */
//static void timeout1(void *parameter)
//{
//
//
//    if(lockflag == 1)
//    {
//         for (int i = 0; i < 10; ++i)
//         {
//             rt_kprintf("periodic timer is timeout %d\n", cnt);
//                 /* 运行第10次，停止周期定时器 */
//             cnt++;
//         }
//         rt_hw_us_delay(1000);
//         init();
//    }
//
//    else
//    {
//
//    }
//
//
//}
//
//int timer_sample(void)
//{
//    /* 创建定时器1  周期定时器 */
//    timer1 = rt_timer_create("timer1", timeout1,
//                             RT_NULL, 1000,
//                             RT_TIMER_FLAG_PERIODIC);
//    /* 启动定时器1 */
//    if (timer1 != RT_NULL)
//        rt_timer_start(timer1);
//
//    return 0;
//}
///* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(timer_sample, timer sample);
