/* USER CODE BEGIN Header */
/*
 * FreeRTOS Kernel V10.3.1
 * Portion Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Portion Copyright (C) 2019 StMicroelectronics, Inc.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
/* USER CODE END Header */

/**
  * @file    FreeRTOSConfig.h
  * @brief   FreeRTOS内核配置文件——定义调度策略、内存管理、任务参数、中断优先级等核心配置项
  *          基于STM32F411 + CMSIS-RTOS V2封装，使用Heap4内存管理方案
  */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * These parameters and more are described within the 'configuration' section of the
 * FreeRTOS API documentation available on the FreeRTOS.org web site.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* USER CODE BEGIN Includes */
/* Section where include file can be added */
/* USER CODE END Includes */

/* Ensure definitions are only used by the compiler, and not by the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
  #include <stdint.h>
  extern uint32_t SystemCoreClock;
#endif
#ifndef CMSIS_device_header
#define CMSIS_device_header "stm32f4xx.h"
#endif /* CMSIS_device_header */

// ====== 内核基础配置 ======
// FPU和MPU：禁用以节省资源（智能手表场景不需要）
#define configENABLE_FPU                         0   // 禁用硬件浮点单元
#define configENABLE_MPU                         0   // 禁用内存保护单元

#define configUSE_PREEMPTION                     1   // 使能抢占式调度（高优先级任务可抢占低优先级）
#define configSUPPORT_STATIC_ALLOCATION          1   // 支持静态内存分配
#define configSUPPORT_DYNAMIC_ALLOCATION         1   // 支持动态内存分配（vTaskCreate等使用）
#define configUSE_IDLE_HOOK                      1   // 使能空闲任务钩子函数（用于低功耗处理等）
#define configUSE_TICK_HOOK                      1   // 使能Tick钩子函数（用于定时任务更新）
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )  // CPU时钟频率 = HCLK（100MHz）
#define configTICK_RATE_HZ                       ((TickType_t)1000)   // 系统滴答频率 = 1000Hz（1ms周期）
#define configMAX_PRIORITIES                     ( 56 )   // 最大任务优先级数（CMSIS-RTOS V2要求≥56）
#define configMINIMAL_STACK_SIZE                 ((uint16_t)128)  // 最小任务栈大小（字为单位，128字=512字节）
#define configTOTAL_HEAP_SIZE                    ((size_t)16*1024)  // 堆总大小 = 16KB（用于动态内存分配）
#define configMAX_TASK_NAME_LEN                  ( 16 )   // 任务名最大长度（含'\0'）
#define configUSE_TRACE_FACILITY                 1        // 使能跟踪功能（用于调试和运行时统计）
#define configUSE_16_BIT_TICKS                   0        // 使用32位Tick计数器（避免溢出问题）
#define configUSE_MUTEXES                        1        // 使能互斥量
#define configQUEUE_REGISTRY_SIZE                8        // 队列注册表大小（用于调试队列信息）
#define configUSE_RECURSIVE_MUTEXES              1        // 使能递归互斥量
#define configUSE_COUNTING_SEMAPHORES            1        // 使能计数信号量
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0        // 使用通用任务选择算法（非硬件CLZ优化）
/* USER CODE BEGIN MESSAGE_BUFFER_LENGTH_TYPE */
/* Defaults to size_t for backward compatibility, but can be changed
   if lengths will always be less than the number of bytes in a size_t. */
#define configMESSAGE_BUFFER_LENGTH_TYPE         size_t   // 消息缓冲区长度类型
/* USER CODE END MESSAGE_BUFFER_LENGTH_TYPE */

/* Co-routine definitions. */
// 协程配置（本项目未使用协程，全部使用任务）
#define configUSE_CO_ROUTINES                    0        // 禁用协程
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )    // 协程最大优先级（未使用）

/* Software timer definitions. */
// 软件定时器配置
#define configUSE_TIMERS                         1        // 使能软件定时器
#define configTIMER_TASK_PRIORITY                ( 2 )    // 定时器服务任务优先级
#define configTIMER_QUEUE_LENGTH                 10       // 定时器命令队列长度
#define configTIMER_TASK_STACK_DEPTH             256      // 定时器服务任务栈深度（字）

/* CMSIS-RTOS V2 flags */
// CMSIS-RTOS V2 API兼容层配置
#define configUSE_OS2_THREAD_SUSPEND_RESUME  1   // 支持线程挂起/恢复API
#define configUSE_OS2_THREAD_ENUMERATE       1   // 支持线程枚举API
#define configUSE_OS2_EVENTFLAGS_FROM_ISR    1   // 支持从中断设置事件标志
#define configUSE_OS2_THREAD_FLAGS           1   // 支持线程标志
#define configUSE_OS2_TIMER                  1   // 支持CMSIS定时器API
#define configUSE_OS2_MUTEX                  1   // 支持CMSIS互斥量API

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
// FreeRTOS API函数包含/排除控制（减小代码体积）
#define INCLUDE_vTaskPrioritySet             1   // 包含vTaskPrioritySet()
#define INCLUDE_uxTaskPriorityGet            1   // 包含uxTaskPriorityGet()
#define INCLUDE_vTaskDelete                  1   // 包含vTaskDelete()
#define INCLUDE_vTaskCleanUpResources        0   // 不包含清理资源函数（节省代码空间）
#define INCLUDE_vTaskSuspend                 1   // 包含vTaskSuspend()
#define INCLUDE_vTaskDelayUntil              1   // 包含vTaskDelayUntil()
#define INCLUDE_vTaskDelay                   1   // 包含vTaskDelay()
#define INCLUDE_xTaskGetSchedulerState       1   // 包含xTaskGetSchedulerState()
#define INCLUDE_xTimerPendFunctionCall       1   // 包含xTimerPendFunctionCall()
#define INCLUDE_xQueueGetMutexHolder         1   // 包含xQueueGetMutexHolder()
#define INCLUDE_uxTaskGetStackHighWaterMark  1   // 包含uxTaskGetStackHighWaterMark()
#define INCLUDE_xTaskGetCurrentTaskHandle    1   // 包含xTaskGetCurrentTaskHandle()
#define INCLUDE_eTaskGetState                1   // 包含eTaskGetState()

/*
 * The CMSIS-RTOS V2 FreeRTOS wrapper is dependent on the heap implementation used
 * by the application thus the correct define need to be enabled below
 */
// 内存管理方案：Heap4（最佳匹配算法，支持内存合并，抗碎片化）
#define USE_FreeRTOS_HEAP_4

/* Cortex-M specific definitions. */
// Cortex-M4中断优先级配置
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS         __NVIC_PRIO_BITS   // 优先级位数（STM32F4 = 4位，0-15共16级）
#else
 #define configPRIO_BITS         4                    // 默认4位优先级
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
// 最低中断优先级（数值最大，优先级最低）：15
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
// 可调用FreeRTOS中断安全API的最高中断优先级：5
// 优先级高于5（数值<5）的中断禁止调用FreeRTOS API函数
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
// 内核层中断优先级计算（移位到PRI寄存器对应的位域位置）
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
/* USER CODE BEGIN 1 */
// 断言宏定义：断言失败时禁用中断并进入死循环
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}
/* USER CODE END 1 */

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
// FreeRTOS内核中断向量映射
#define vPortSVCHandler    SVC_Handler       // SVC系统服务调用映射
#define xPortPendSVHandler PendSV_Handler    // PendSV（用于任务上下文切换）映射

/* IMPORTANT: After 10.3.1 update, Systick_Handler comes from NVIC (if SYS timebase = systick), otherwise from cmsis_os2.c */

// 使用自定义SysTick处理函数（本项目使用TIM1作为时基，不使用SysTick）
#define USE_CUSTOM_SYSTICK_HANDLER_IMPLEMENTATION 0

/* USER CODE BEGIN Defines */
// 用户自定义配置：使能运行时统计信息格式化函数（vTaskGetRunTimeStats等）
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

/* Section where parameter definitions can be added (for instance, to override default ones in FreeRTOS.h) */
/* USER CODE END Defines */

#endif /* FREERTOS_CONFIG_H */
