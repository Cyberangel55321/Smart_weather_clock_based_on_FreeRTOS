#ifndef __FREERTOS_CONFIG_H__
#define __FREERTOS_CONFIG_H__

/* Here is a good place to include header files that are required across
   your application. */
#include "stm32f4xx.h"
extern uint32_t SystemCoreClock;

#define configUSE_PREEMPTION                                        1   // 是否开启抢占式调度
#define configUSE_PORT_OPTIMISED_TASK_SELECTION                     1   // 选择任务调度算法, 1为使用位图算法, 0为使用线性搜索算法
#define configUSE_TICKLESS_IDLE                                     0   // 是否使用低功耗tickless idle模式
#define configCPU_CLOCK_HZ                                          SystemCoreClock  // CPU时钟频率
#define configTICK_RATE_HZ                                          1000   // 时钟节拍频率, 即每秒钟的节拍数
#define configMAX_PRIORITIES                                        10  // 可使用的最大优先级数, 取值必须大于0
#define configMINIMAL_STACK_SIZE                                    128 // 定义空闲任务的堆栈大小, 以字为单位, 128即512字节
#define configMAX_TASK_NAME_LEN                                     16  // 任务名字的最大长度, 以字节为单位, 16即最多支持15个字符的名字(最后一个字节用于存储字符串结束符'\0')
#define configUSE_16_BIT_TICKS                                      0   // 是否使用16位的tick计数器, 0为使用32位, 1为使用16位
#define configIDLE_SHOULD_YIELD                                     1   // 当空闲任务被唤醒时, 是否让出CPU给优先级更高的任务, 0为不让出, 1为让出
#define configUSE_TASK_NOTIFICATIONS                                1   // 是否使用任务通知功能, 任务通知是一种轻量级的线程间通信机制, 可以用来替代信号量和事件组等功能
#define configTASK_NOTIFICATION_ARRAY_ENTRIES                       3   // 每个任务的通知数组长度, 只有当configUSE_TASK_NOTIFICATIONS为1时才有效, 默认为3
#define configUSE_MUTEXES                                           1   // 是否使用互斥锁, 互斥锁是一种特殊的二值信号量, 用于保护共享资源的访问, 可以避免优先级反转问题
#define configUSE_RECURSIVE_MUTEXES                                 1   // 是否使用递归互斥锁, 递归互斥锁允许同一个任务多次获取同一个互斥锁而不会导致死锁, 只有当configUSE_MUTEXES为1时才有效
#define configUSE_COUNTING_SEMAPHORES                               1   // 是否使用计数信号量, 计数信号量是一种可以取值大于1的信号量, 用于控制对多个实例资源的访问
#define configUSE_ALTERNATIVE_API                                   0   // 是否使用替代API, 替代API是一组与原有API功能相同但参数不同的函数, 已经废弃, 不建议使用
#define configQUEUE_REGISTRY_SIZE                                   10  // 队列注册表的大小, 队列注册表是一个全局数组, 用于存储系统中所有创建的队列和信号量的名字和句柄, 以便在调试时查看
#define configUSE_QUEUE_SETS                                        1   // 是否使用队列集合, 队列集合是一种特殊的数据结构, 可以同时包含多个队列和信号量, 允许任务等待集合中任意一个对象变为可用, 只有当configUSE_QUEUE_SETS为1时才有效
#define configUSE_TIME_SLICING                                      1   // 是否使用时间片轮转调度, 当多个同优先级的任务同时就绪时, 是否让它们轮流占用CPU, 0为不使用, 1为使用
#define configUSE_NEWLIB_REENTRANT                                  0   // 是否使用Newlib库的可重入版本, Newlib是一个C标准库实现, 可重入版本允许在多线程环境下安全地使用库函数, 只有当系统使用Newlib库时才有效
#define configENABLE_BACKWARD_COMPATIBILITY                         0   // 是否启用向后兼容性, 启用后可以使用一些已经废弃的API和特性, 但不建议启用, 因为可能会导致代码臃肿和性能下降
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS                     5   // 每个任务的线程局部存储指针数量, 线程局部存储是一种为每个任务提供独立数据存储的机制, 可以用来存储任务特定的数据, 只有当系统需要使用线程局部存储功能时才有效
#define configUSE_MINI_LIST_ITEM                                    1   // 是否使用迷你列表项, 迷你列表项是一种优化的数据结构, 用于减少任务控制块和队列项的内存占用, 只有当configUSE_PORT_OPTIMISED_TASK_SELECTION为1时才有效
#define configSTACK_DEPTH_TYPE                                      uint32_t  // 定义堆栈深度的数据类型, 以字为单位, 默认为uint32_t
#define configMESSAGE_BUFFER_LENGTH_TYPE                            size_t // 定义消息缓冲区长度的数据类型, 默认为size_t

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION                             0   // 是否支持静态内存分配, 0为不支持, 1为支持
#define configSUPPORT_DYNAMIC_ALLOCATION                            1   // 是否支持动态内存分配, 0为不支持, 1为支持
#define configTOTAL_HEAP_SIZE                                       (92 * 1024)   // 定义堆的总大小, 单位为字节
#define configAPPLICATION_ALLOCATED_HEAP                            0   // 应用程序是否负责分配堆内存, 0为不负责, 1为负责

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                                 0  // 是否使用空闲钩子函数, 空闲钩子函数是在系统空闲时调用的函数, 可以用来执行一些低优先级的任务或进入低功耗模式
#define configUSE_TICK_HOOK                                 0  // 是否使用时钟节拍钩子函数, 时钟节拍钩子函数是在每个时钟节拍中调用的函数, 可以用来执行一些周期性的任务或监控系统状态
#define configCHECK_FOR_STACK_OVERFLOW                      1  // 是否启用堆栈溢出检查, 0为不启用, 1为使用方法1进行检查, 2为使用方法2进行检查, 方法1是在每个任务切换时检查堆栈指针是否超出边界, 方法2是在每个任务切换时检查堆栈中是否存在特定的标记值
#define configUSE_MALLOC_FAILED_HOOK                        1  // 是否使用内存分配失败钩子函数, 内存分配失败钩子函数是在动态内存分配失败时调用的函数, 可以用来记录错误信息或重启系统
#define configUSE_DAEMON_TASK_STARTUP_HOOK                  0  // 是否使用守护任务启动钩子函数, 守护任务启动钩子函数是在守护任务创建时调用的函数, 可以用来执行一些初始化操作或创建其他任务
#define configUSE_SB_COMPLETED_CALLBACK                     0  // 是否使用软件定时器完成回调函数, 软件定时器完成回调函数是在软件定时器到期时调用的函数, 可以用来执行一些定时任务或通知其他任务

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS                       0  // 是否生成运行时间统计信息, 0为不生成, 1为生成, 生成后可以使用vTaskGetRunTimeStats()函数获取每个任务的运行时间和CPU占用率等信息
#define configUSE_TRACE_FACILITY                            0  // 是否使用跟踪设施, 跟踪设施是一种调试工具, 可以用来记录系统中发生的事件和状态变化, 以便分析和优化系统性能
#define configUSE_STATS_FORMATTING_FUNCTIONS                0  // 是否使用统计格式化函数, 统计格式化函数是一组用于格式化运行时间统计信息的函数, 只有当configGENERATE_RUN_TIME_STATS为1时才有效

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                               0  // 是否使用协程, 协程是一种轻量级的线程, 只能在单核处理器上运行, 不能被抢占, 只能通过协作式调度来切换, 0为不使用, 1为使用
#define configMAX_CO_ROUTINE_PRIORITIES                     1  // 协程的最大优先级数, 取值必须大于0, 只有当configUSE_CO_ROUTINES为1时才有效

/* Software timer related definitions. */
#define configUSE_TIMERS                                    1  // 是否使用软件定时器, 软件定时器是一种在软件中实现的定时器机制, 可以用来执行一些定时任务或延迟操作, 0为不使用, 1为使用
#define configTIMER_TASK_PRIORITY                           (configMAX_PRIORITIES - 1) // 软件定时器任务的优先级, 通常设置为最高优先级, 以确保定时器的准确性和及时性
#define configTIMER_QUEUE_LENGTH                            32 // 软件定时器队列的长度, 定义了同时可以有多少个定时器事件在等待处理
#define configTIMER_TASK_STACK_DEPTH                        configMINIMAL_STACK_SIZE   // 软件定时器任务的堆栈大小, 以字为单位, 通常设置为最小堆栈大小的两倍或更多, 以确保定时器任务有足够的堆栈空间来处理定时器事件

/* Interrupt nesting behaviour configuration. */
#define configPRIO_BITS                         4  // 定义用于表示中断优先级的位数, STM32F4系列通常为4位, 取值范围为0-15, 0为最高优先级, 15为最低优先级
#define configKERNEL_INTERRUPT_PRIORITY         (15 << (8 - configPRIO_BITS)) // 定义内核中断优先级, 取值必须为configPRIO_BITS位数范围内的值, 通常设置为最低优先级, 以确保内核中断不会干扰应用程序的中断
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (5 << (8 - configPRIO_BITS))  // 定义可调用系统调用的最高中断优先级, 取值必须为configPRIO_BITS位数范围内的值, 通常设置为较高优先级, 以确保系统调用能够及时响应, 但不会干扰内核中断
#define configMAX_API_CALL_INTERRUPT_PRIORITY   configMAX_SYSCALL_INTERRUPT_PRIORITY   // 定义可调用API的最高中断优先级, 取值必须为configPRIO_BITS位数范围内的值, 通常设置为与configMAX_SYSCALL_INTERRUPT_PRIORITY相同, 以确保API能够及时响应, 但不会干扰内核中断

/* Define to trap errors during development. */
void vAssertCalled(const char *file, int line);
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                1  // 是否包含vTaskPrioritySet()函数, 该函数用于设置任务的优先级
#define INCLUDE_uxTaskPriorityGet               1  // 是否包含uxTaskPriorityGet()函数, 该函数用于获取任务的优先级
#define INCLUDE_vTaskDelete                     1  // 是否包含vTaskDelete()函数, 该函数用于删除一个任务
#define INCLUDE_vTaskSuspend                    1  // 是否包含vTaskSuspend()函数, 该函数用于挂起一个任务
#define INCLUDE_vTaskDelayUntil                 1  // 是否包含vTaskDelayUntil()函数, 该函数用于延迟一个任务直到指定的时间点
#define INCLUDE_vTaskDelay                      1  // 是否包含vTaskDelay()函数, 该函数用于延迟一个任务指定的时间量
#define INCLUDE_xTaskGetSchedulerState          1  // 是否包含xTaskGetSchedulerState()函数, 该函数用于获取调度器的状态, 可以返回调度器是运行、挂起还是未启动
#define INCLUDE_xTaskGetCurrentTaskHandle       1  // 是否包含xTaskGetCurrentTaskHandle()函数, 该函数用于获取当前任务的句柄
#define INCLUDE_uxTaskGetStackHighWaterMark     1  // 是否包含uxTaskGetStackHighWaterMark()函数, 该函数用于获取任务堆栈的最小剩余空间, 可以用来检测堆栈溢出风险
#define INCLUDE_uxTaskGetStackHighWaterMark2    1  // 是否包含uxTaskGetStackHighWaterMark2()函数, 该函数用于获取任务堆栈的最小剩余空间, 以字节为单位, 只有当configSTACK_DEPTH_TYPE为uint32_t时才有效
#define INCLUDE_xTaskGetIdleTaskHandle          1  // 是否包含xTaskGetIdleTaskHandle()函数, 该函数用于获取空闲任务的句柄
#define INCLUDE_eTaskGetState                   1  // 是否包含eTaskGetState()函数, 该函数用于获取任务的状态, 可以返回任务是运行、就绪、阻塞、挂起还是删除
#define INCLUDE_xTimerPendFunctionCall          0  // 是否包含xTimerPendFunctionCall()函数, 该函数用于在定时器服务任务的上下文中执行一个函数, 只有当configUSE_TIMERS为1时才有效
#define INCLUDE_xTaskAbortDelay                 0  // 是否包含xTaskAbortDelay()函数, 该函数用于中止一个任务的延迟, 只有当configUSE_TASK_NOTIFICATIONS为1时才有效
#define INCLUDE_xTaskGetHandle                  1  // 是否包含xTaskGetHandle()函数, 该函数用于根据任务名字获取任务的句柄
#define INCLUDE_xTaskResumeFromISR              1  // 是否包含xTaskResumeFromISR()函数, 该函数用于从中断服务程序中恢复一个任务, 只有当configUSE_TASK_NOTIFICATIONS为1时才有效

/* A header file that defines trace macro can be included here. */

#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler
#define vPortSVCHandler SVC_Handler

#endif /* FREERTOS_CONFIG_H */
