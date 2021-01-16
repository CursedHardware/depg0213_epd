#ifndef __depg0213_EPD_H
#define __depg0213_EPD_H

#include <stdint.h>

#define DEPG0213_PANEL_SELECTION depg0213_dke_init_sequence

#ifndef DEPG0213_LUT_OTP
#define DEPG0213_LUT_OTP 0
#endif

#if(!DEPG0213_LUT_OTP)

#define DEPG0213_LUT_FULL_SELECTION depg0213_dke_lut_full
#define DEPG0213_LUT_PART_SELECTION depg0213_dke_lut_part

#endif

typedef enum {
    DEPG0213_OK,
    DEPG0213_ERROR
} depg0213_ret_t;

typedef enum {
    DEPG0213_VERTICAL,
    DEPG0213_HORIZONTAL,
    DEPG0213_VERTICAL_INVERSE,
    DEPG0213_HORIZONTAL_INVERSE
} depg0213_direction_t;

typedef struct {
    depg0213_ret_t (*reset_cb)(void *handle);
    depg0213_ret_t (*write_cmd_cb)(void *handle, uint8_t *cmd, uint8_t len);
    depg0213_ret_t (*write_data_cb)(void *handle, uint8_t *data, uint16_t len);
    depg0213_ret_t (*poll_busy_cb)(void *handle);
} depg0213_cb_t;

typedef struct {
    void *user_data;
    uint8_t deep_sleep;
    depg0213_direction_t direction;
    depg0213_cb_t cb;
} depg0213_epd_t;

#define DEPG0213_ERROR_CHECK(x) if(x != DEPG0213_OK) return DEPG0213_ERROR

depg0213_ret_t depg0213_epd_init(depg0213_epd_t *epd);
depg0213_ret_t depg0213_epd_update(depg0213_epd_t *epd);
depg0213_ret_t depg0213_epd_load(depg0213_epd_t *epd, uint8_t *bw_image, uint8_t *red_image);
depg0213_ret_t depg0213_epd_deepsleep(depg0213_epd_t *epd);
depg0213_ret_t depg0213_epd_direction(depg0213_epd_t *epd, depg0213_direction_t direction);

#endif