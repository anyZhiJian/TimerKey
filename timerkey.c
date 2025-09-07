// SPDX-License-Identifier: MIT
// Copyright (c) 2025 yanzhijian

#include "timerkey.h"

#define TKEY_STATE_UNPRESSED 0
#define TKEY_STATE_PRESSED 1
#define TKEY_MAX_TICKS (0xFFFF)
#define TKEY_MAX_COUNT (0xFF)

struct tkey_t {
    tkey_event_cb_t event_cb;
    tkey_detect_cb_t detect_cb;
    void *user_data;
    uint16_t hold_ticks;
    uint16_t debounce_ticks;
    uint16_t multi_press_interval_ticks;
    uint16_t pressed_ticks;
    uint16_t multi_press_ticks;
    uint8_t multi_press_count;
    uint8_t press_state : 1;
    uint8_t pressed_level : 1;
    uint8_t long_pressed_flag : 1;
    uint8_t enabled : 1;
};

tkey_handle_t tkey_create(tkey_config_t *config) {
    if (!config)
        return NULL;
    tkey_handle_t tkey = (tkey_handle_t)tkey_malloc(sizeof(struct tkey_t));
    if (!tkey)
        return NULL;
    tkey->event_cb = config->event_cb;
    tkey->detect_cb = config->detect_cb;
    tkey->user_data = config->user_data;
    tkey->hold_ticks = config->hold_ticks;
    tkey->debounce_ticks = config->debounce_ticks;
    tkey->multi_press_interval_ticks = config->multi_press_interval_ticks;
    tkey->pressed_level = config->pressed_level;
    tkey->pressed_ticks = 0;
    tkey->multi_press_ticks = 0;
    tkey->multi_press_count = 0;
    tkey->press_state = TKEY_STATE_UNPRESSED;
    tkey->long_pressed_flag = 0;
    tkey->enabled = 1;
    return tkey;
}

void tkey_delete(tkey_handle_t key) {
    if (!key)
        return;
    tkey_free(key);
    key = NULL;
}

tkey_handle_t tkey_create_default(tkey_event_cb_t event_cb,
                                  tkey_detect_cb_t detect_cb, void *user_data) {
    tkey_config_t config;
    config.debounce_ticks = 1;
    config.detect_cb = NULL;
    config.event_cb = NULL;
    config.hold_ticks = 25;
    config.multi_press_interval_ticks = 15;
    config.pressed_level = 0;
    config.user_data = NULL;
    tkey_handle_t key = tkey_create(&config);
    if (!key)
        return NULL;
    tkey_register_callback(key, event_cb, detect_cb, user_data);
    return key;
}

void tkey_handler(tkey_handle_t key) {
    if (!key)
        return;
    if (!key->enabled || !key->detect_cb || !key->event_cb)
        return;
    if (key->multi_press_count > 0) {
        if (key->multi_press_ticks < TKEY_MAX_TICKS)
            key->multi_press_ticks++;
        if (key->multi_press_ticks > key->multi_press_interval_ticks) {
            key->multi_press_count = 0;
            key->multi_press_ticks = 0;
        }
    }
    if (key->press_state == TKEY_STATE_UNPRESSED) {
        if (key->detect_cb(key->user_data) == key->pressed_level) {
            if (key->pressed_ticks >= key->debounce_ticks) {
                key->press_state = TKEY_STATE_PRESSED;
                key->pressed_ticks = 0;
                key->multi_press_ticks = 0;
                if (key->multi_press_count < TKEY_MAX_COUNT)
                    key->multi_press_count++;
                if (key->multi_press_count > 1)
                    key->event_cb(key, TKEY_EVENT_MULTI_PRESS,
                                  key->multi_press_count, key->user_data);
                else
                    key->event_cb(key, TKEY_EVENT_PRESS, key->multi_press_count,
                                  key->user_data);
            }
            if (key->pressed_ticks < TKEY_MAX_TICKS)
                key->pressed_ticks++;
        }
    } else if (key->press_state == TKEY_STATE_PRESSED) {
        if (key->pressed_ticks < TKEY_MAX_TICKS)
            key->pressed_ticks++;
        if (key->detect_cb(key->user_data) != key->pressed_level) {
            key->press_state = TKEY_STATE_UNPRESSED;
            if (key->multi_press_count > 1)
                key->event_cb(key, TKEY_EVENT_MULTI_RELEASE,
                              key->multi_press_count, key->user_data);
            else if (key->long_pressed_flag) {
                key->long_pressed_flag = 0;
                key->event_cb(key, TKEY_EVENT_LONG_RELEASE,
                              key->multi_press_count, key->user_data);
            } else
                key->event_cb(key, TKEY_EVENT_RELEASE, key->multi_press_count,
                              key->user_data);
            key->pressed_ticks = 0;
        } else if (key->pressed_ticks == key->hold_ticks) {
            key->long_pressed_flag = 1;
            key->event_cb(key, TKEY_EVENT_LONG_PRESS, key->multi_press_count,
                          key->user_data);
        }
    }
}

void tkey_multi_handler(tkey_handle_t key[], uint32_t num) {
    uint32_t i = num;
    while (i--) {
        tkey_handler(key[i]);
    }
}

void tkey_register_callback(tkey_handle_t key, tkey_event_cb_t event_cb,
                            tkey_detect_cb_t detect_cb, void *user_data) {
    if (!key)
        return;
    key->user_data = user_data;
    key->event_cb = event_cb;
    key->detect_cb = detect_cb;
}

void tkey_set_pressed_level(tkey_handle_t key, uint8_t pressed_level) {
    if (!key)
        return;
    key->pressed_level = pressed_level;
}

void tkey_set_hold(tkey_handle_t key, uint16_t hold_ticks) {
    if (!key)
        return;
    key->hold_ticks = hold_ticks;
}

void tkey_set_debounce(tkey_handle_t key, uint16_t debounce_ticks) {
    if (!key)
        return;
    key->debounce_ticks = debounce_ticks;
}

void tkey_set_multi_press_interval(tkey_handle_t key,
                                   uint16_t multi_press_interval_ticks) {
    if (!key)
        return;
    key->multi_press_interval_ticks = multi_press_interval_ticks;
}

void tkey_set_enabled(tkey_handle_t key, uint8_t enabled) {
    if (!key)
        return;
    key->enabled = enabled;
}