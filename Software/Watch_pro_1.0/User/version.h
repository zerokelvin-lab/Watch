/** @file version.h @brief 版本号定义 */
#ifndef VERSION_H
#define VERSION_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************
 * CURRENT VERSION OF OV-Watch
 * 当前 OV-Watch 版本号
 ***************************/
/**
 * 版本号规则：
 * 整体机械结构变更 → 主版本号 (major)
 * 硬件变更 → 次版本号 (minor)
 * 软件变更 → 修订版本号 (patch)
 *
 * total mechanical structure change will show in major
 * hardware change will show in minor
 * software change will show in patch
 *
 */
#define VERSION_MAJOR 1      // 主版本号
#define VERSION_MINOR 1      // 次版本号
#define VERSION_PATCH 0      // 修订版本号
#define VERSION_INFO "LCD flush use synchronous DMA, fixed some bugs."  // 版本变更说明

#define VERSION_AUTHOR_ENG_NAME    "zerokelvin"
#define VERSION_AUTHOR_CN_NAME     "不吃油炸鸡"
#define VERSION_PROJECT_LINK       "https://github.com/No-Chicken/OV-Watch"  // 项目链接


/**
 * Wrapper functions for VERSION macros
 * 版本宏的封装函数，通过函数接口获取版本信息
 */

static inline int watch_version_major(void)  // 获取主版本号
{
    return VERSION_MAJOR;
}

static inline int watch_version_minor(void)  // 获取次版本号
{
    return VERSION_MINOR;
}

static inline int watch_version_patch(void)  // 获取修订版本号
{
    return VERSION_PATCH;
}

static inline const char *watch_version_info(void)  // 获取版本变更说明
{
    return VERSION_INFO;
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
