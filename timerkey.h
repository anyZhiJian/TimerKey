#ifndef __TIMERKEY_H
#define __TIMERKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#define tkey_malloc(size) malloc(size)
#define tkey_free(ptr) free(ptr)

typedef enum {
    TKEY_EVENT_PRESS = 0x01,
    TKEY_EVENT_LONG_PRESS = 0x02,
    TKEY_EVENT_RELEASE = 0x04,
    TKEY_EVENT_LONG_RELEASE = 0x08,
    TKEY_EVENT_MULTI_PRESS = 0x10,
    TKEY_EVENT_MULTI_RELEASE = 0x20,
    TKEY_EVENT_NORMAL_PRESS = (TKEY_EVENT_PRESS | TKEY_EVENT_MULTI_PRESS),
    TKEY_EVENT_ALL_PRESS =
        (TKEY_EVENT_PRESS | TKEY_EVENT_LONG_PRESS | TKEY_EVENT_MULTI_PRESS),
    TKEY_EVENT_ALL_RELEASE = (TKEY_EVENT_RELEASE | TKEY_EVENT_LONG_RELEASE |
                              TKEY_EVENT_MULTI_RELEASE),
} tkey_event_t;

typedef struct tkey_t tkey_t;

typedef void (*tkey_event_cb_t)(tkey_t *key, tkey_event_t event,
                                uint8_t press_count, void *user_data);
typedef int (*tkey_detect_cb_t)(void *user_data);

typedef struct {
    tkey_event_cb_t event_cb;
    tkey_detect_cb_t detect_cb;
    void *user_data;
    uint16_t hold_ticks;
    uint16_t debounce_ticks;
    uint16_t multi_press_interval_ticks;
    uint16_t pressed_level;
} tkey_config_t;

tkey_t *tkey_create_default(tkey_event_cb_t event_cb,
                            tkey_detect_cb_t detect_cb, void *user_data);
tkey_t *tkey_create(tkey_config_t *config);
void tkey_delete(tkey_t *key);
void tkey_handler(tkey_t *key);
void tkey_register_callback(tkey_t *key, tkey_event_cb_t event_cb,
                            tkey_detect_cb_t detect_cb, void *user_data);
void tkey_set_pressed_level(tkey_t *key, uint8_t pressed_level);
void tkey_set_hold(tkey_t *key, uint16_t hold_ticks);
void tkey_set_debounce(tkey_t *key, uint16_t debounce_ticks);
void tkey_set_multi_press_interval(tkey_t *key,
                                   uint16_t multi_press_interval_ticks);

#ifdef __cplusplus
}
#endif

#endif