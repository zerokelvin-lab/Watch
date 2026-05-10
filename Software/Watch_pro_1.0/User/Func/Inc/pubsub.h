/** @file pubsub.h
 *  @brief 发布-订阅系统，实现消息的发布与订阅机制，支持多订阅者注册和消息广播
 */

#ifndef PUBSUB_H
#define PUBSUB_H

#include <stdint.h>

// 消息结构体，封装消息ID和数据内容
typedef struct {
    int id;             // 消息ID，用于标识消息类型
    char data[256];     // 消息数据负载，最大256字节
} PubSub_Message_t;

// 订阅者回调函数指针类型
typedef void (*Subscriber)(PubSub_Message_t);

// 订阅者链表节点，用于构建订阅者链
typedef struct SubscriberNode {
    Subscriber subscriber;         // 订阅者回调函数
    struct SubscriberNode *next;   // 指向下一个节点的指针
} SubscriberNode;

// 发布者结构体，维护一个订阅者链表
typedef struct {
    SubscriberNode *subscribers; // 订阅者链表头指针
} PubSub_Publisher_t;

// ====== 发布-订阅API函数声明 ======
void Publisher_init(PubSub_Publisher_t *publisher);                               // 初始化发布者，清空订阅者链表
void Publisher_subscribe(PubSub_Publisher_t *publisher, Subscriber subscriber);   // 注册订阅者
void Publisher_unsubscribe(PubSub_Publisher_t *publisher, Subscriber subscriber); // 取消订阅
void Publisher_publish(PubSub_Publisher_t *publisher, PubSub_Message_t message);  // 发布消息，通知所有订阅者

extern PubSub_Publisher_t SDL_KeyBoard_Publisher; // SDL键盘事件发布者实例

#endif // PUBSUB_H
