/** @file user_Queue.h @brief 循环队列数据结构定义，用于心率算法的滑动窗口数据缓存 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"

/** @brief 队列最大容量（心率算法滑动窗口=7） */
#define QUEUE_SIZE 7

/** @brief 循环队列结构体 */
typedef struct {
    int8_t front;              // 队首索引
    int8_t rear;               // 队尾索引
    int8_t size;               // 当前队列元素数量
    uint32_t data[QUEUE_SIZE]; // 数据缓冲区
} Queue;

void initQueue(Queue *queue);                            /** @brief 初始化队列 */
bool isQueueEmpty(Queue *queue);                         /** @brief 判断队列是否为空 */
bool isQueueFull(Queue *queue);                          /** @brief 判断队列是否已满 */
void enqueue(Queue *queue, unsigned long item);           /** @brief 入队操作 @param item 入队元素 */
uint32_t dequeue(Queue *queue);                          /** @brief 出队操作 @return 出队元素 */
