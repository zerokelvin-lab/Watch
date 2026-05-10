/** @file user_Queue.c @brief 循环队列实现，用于心率算法的滑动窗口数据缓存 */
#include "user_Queue.h"


/**
  * @brief 初始化循环队列
  * @param queue 队列指针
  */
void initQueue(Queue *queue) 
{
    queue->front = 0;   // 队首初始为0
    queue->rear = -1;   // 队尾初始为-1(表示空队列)
    queue->size = 0;    // 元素数量为0
}

/**
  * @brief 判断队列是否为空
  * @param queue 队列指针
  * @return true=队列为空, false=非空
  */
bool isQueueEmpty(Queue *queue) 
{
    return queue->size == 0;
}

/**
  * @brief 判断队列是否已满
  * @param queue 队列指针
  * @return true=队列已满, false=未满
  */
bool isQueueFull(Queue *queue) 
{
    return queue->size == QUEUE_SIZE;
}

/**
  * @brief 循环队列入队操作（尾插法）
  * @param queue 队列指针
  * @param item 入队元素
  */
void enqueue(Queue *queue, unsigned long item) 
{
    if (isQueueFull(queue)) 
    {
        printf("队列已满，无法入队！\n");  // 队列已满，打印提示
        return;
    }
    // 队尾指针循环+1后存入数据
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->data[queue->rear] = item;
    queue->size++;
}

/**
  * @brief 循环队列出队操作（头删法）
  * @param queue 队列指针
  * @return 出队的元素值
  */
uint32_t dequeue(Queue *queue) 
{
    if (isQueueEmpty(queue)) 
    {
        printf("队列为空，无法出队！\n");  // 队列为空，打印提示
        return 0;
    }
    // 取出队首元素，队首指针循环+1
    unsigned long item = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->size--;
    return item;
}
