/** @file pubsub.c
 *  @brief 发布-订阅系统实现——基于链表管理订阅者，支持订阅、取消订阅和消息广播
 */

#include "../Inc/pubsub.h"
#include <stdlib.h>
#include <stdio.h>

PubSub_Publisher_t SDL_KeyBoard_Publisher; // SDL键盘事件发布者全局实例

/**
 * @brief  初始化发布者，将订阅者链表置空
 * @param  publisher 发布者指针
 * @return 无
 */
void Publisher_init(PubSub_Publisher_t *publisher) {
    publisher->subscribers = NULL; // 订阅者链表初始化为空
}

/**
 * @brief  注册一个新的订阅者，将其加入链表头部
 * @param  publisher  发布者指针
 * @param  subscriber 订阅者回调函数
 * @return 无
 */
void Publisher_subscribe(PubSub_Publisher_t *publisher, Subscriber subscriber) {
    SubscriberNode *node = (SubscriberNode *)malloc(sizeof(SubscriberNode)); // 分配新节点内存
    if (node == NULL) {
        // Handle memory allocation failure
        return; // 内存分配失败，直接返回
    }
    node->subscriber = subscriber;            // 保存订阅者回调函数
    node->next = publisher->subscribers;      // 新节点指向当前链表头
    publisher->subscribers = node;            // 新节点成为链表头
}

/**
 * @brief  取消订阅——遍历链表找到目标订阅者并移除
 * @param  publisher  发布者指针
 * @param  subscriber 要移除的订阅者回调函数
 * @return 无
 */
void Publisher_unsubscribe(PubSub_Publisher_t *publisher, Subscriber subscriber) {
    SubscriberNode **current = &publisher->subscribers; // 使用二级指针方便链表操作
    while (*current != NULL) {
        if ((*current)->subscriber == subscriber) {      // 找到目标订阅者
            SubscriberNode *to_delete = *current;
            *current = (*current)->next;                  // 从链表中移除该节点
            free(to_delete);                              // 释放节点内存
            return;
        }
        current = &((*current)->next);                    // 移动到下一个节点
    }
}

/**
 * @brief  发布消息——遍历订阅者链表，依次调用每个订阅者的回调函数
 * @param  publisher 发布者指针
 * @param  message   要发布的消息
 * @return 无
 */
void Publisher_publish(PubSub_Publisher_t *publisher, PubSub_Message_t message) {
    SubscriberNode *node = publisher->subscribers;
    while (node != NULL) {
        node->subscriber(message); // 调用当前订阅者的回调函数处理消息
        node = node->next;         // 移动到下一个订阅者
    }
}
