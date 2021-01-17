#include "depg0213_epd.h"

uint8_t depg0213_dke_init_sequence[] = {
    0x01, 0x74, 0x54, // Set analog block control
    0x01, 0x7E, 0x3B, // Set digital block control
    0x03, 0x01, 0xF9, 0x00, 0x00, // Set display size and driver output control
    0x01, 0x3C, 0x01, // Set border
    0x01, 0x2C, 0x5A, // Set VCOM value
    0x01, 0x03, 0x17, // Gate voltage settings
    0x03, 0x04, 0x41, 0xAC, 0x32, // Source voltage settings
    0x01, 0x3A, 0x02, // Frame setting 1
    0x01, 0x3B, 0x0D  // Frame setting 2
};

uint8_t depg0213_dke_lut_full[] = {
    0x80,0x60,0x40,0x00,0x00,0x00,0x00, //LUT0: BB:     VS 0 ~7
    0x10,0x60,0x20,0x00,0x00,0x00,0x00, //LUT1: BW:     VS 0 ~7
    0x80,0x60,0x40,0x00,0x00,0x00,0x00, //LUT2: WB:     VS 0 ~7
    0x10,0x60,0x20,0x00,0x00,0x00,0x00, //LUT3: WW:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00, //LUT4: VCOM:   VS 0 ~7

    0x03,0x03,0x00,0x00,0x02,           // TP0 A~D RP0
    0x09,0x09,0x00,0x00,0x02,           // TP1 A~D RP1
    0x03,0x03,0x00,0x00,0x02,           // TP2 A~D RP2
    0x00,0x00,0x00,0x00,0x00,           // TP3 A~D RP3
    0x00,0x00,0x00,0x00,0x00,           // TP4 A~D RP4
    0x00,0x00,0x00,0x00,0x00,           // TP5 A~D RP5
    0x00,0x00,0x00,0x00,0x00,           // TP6 A~D RP6
};

uint8_t depg0213_dke_lut_part[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00, //LUT0: BB:     VS 0 ~7
    0x80,0x00,0x00,0x00,0x00,0x00,0x00, //LUT1: BW:     VS 0 ~7
    0x40,0x00,0x00,0x00,0x00,0x00,0x00, //LUT2: WB:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00, //LUT3: WW:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00, //LUT4: VCOM:   VS 0 ~7

    0x0A,0x00,0x00,0x00,0x00,           // TP0 A~D RP0
    0x00,0x00,0x00,0x00,0x00,           // TP1 A~D RP1
    0x00,0x00,0x00,0x00,0x00,           // TP2 A~D RP2
    0x00,0x00,0x00,0x00,0x00,           // TP3 A~D RP3
    0x00,0x00,0x00,0x00,0x00,           // TP4 A~D RP4
    0x00,0x00,0x00,0x00,0x00,           // TP5 A~D RP5
    0x00,0x00,0x00,0x00,0x00,           // TP6 A~D RP6
};


depg0213_ret_t _depg0213_software_reset(depg0213_epd_t *epd) {
    uint8_t sw_reset_cmd = 0x12; // SW RST

    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &sw_reset_cmd, 0x01));
    DEPG0213_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));

    return DEPG0213_OK;
}

depg0213_ret_t _depg0213_hardware_reset(depg0213_epd_t *epd) {

    DEPG0213_ERROR_CHECK(epd->cb.reset_cb(epd->user_data));
    DEPG0213_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));

    epd->deep_sleep = 0;

    return DEPG0213_OK;
}

depg0213_ret_t _depg0213_init_seq(depg0213_epd_t *epd) {
   uint16_t i = 0;

    while(i < sizeof(DEPG0213_PANEL_SELECTION)) {
        DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &DEPG0213_PANEL_SELECTION[i + 1], DEPG0213_PANEL_SELECTION[i] + 1));
        i += DEPG0213_PANEL_SELECTION[i] + 2;
    }

    return DEPG0213_OK;
}

depg0213_ret_t _depg0213_load_lut(depg0213_epd_t *epd) {
#if(!DEPG0213_LUT_OTP)
    uint8_t lut_command = 0x32;
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &lut_command, 0x01));
    DEPG0213_ERROR_CHECK(epd->cb.write_data_cb(epd->user_data, DEPG0213_LUT_FULL_SELECTION, 70));
#else
    uint8_t lut_command[5] = { 0x18, 0x80, 0x22, 0xB1, 0x20 };
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, lut_command, 0x02));
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &lut_command[2], 0x02));
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &lut_command[4], 0x01));
    // The end of the sequence is load LUT from OTP memory.
    DEPG0213_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));
#endif

    return DEPG0213_OK;
}

depg0213_ret_t depg0213_epd_init(depg0213_epd_t *epd) {

    // Software reset follows hardware reset
    DEPG0213_ERROR_CHECK(_depg0213_hardware_reset(epd));
    DEPG0213_ERROR_CHECK(_depg0213_software_reset(epd));

    // Send initialization chants
    DEPG0213_ERROR_CHECK(_depg0213_init_seq(epd));

    // Write LUT
    DEPG0213_ERROR_CHECK(_depg0213_load_lut(epd));

    return DEPG0213_OK;
}

depg0213_ret_t depg0213_epd_update(depg0213_epd_t *epd) {

    if(epd->deep_sleep) {
        DEPG0213_ERROR_CHECK(depg0213_epd_init(epd));
    }

    uint8_t command[2] = { 0x22, 0xC7 };
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, command, 0x02));
    command[0] = 0x20;
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, command, 0x01));
    DEPG0213_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));

    return DEPG0213_OK;
}

depg0213_ret_t depg0213_epd_load(depg0213_epd_t *epd, uint8_t *bw_image, uint8_t *red_image,
                                 uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end) {

    if(epd->deep_sleep) {
        DEPG0213_ERROR_CHECK(depg0213_epd_init(epd));
    }

    DEPG0213_ERROR_CHECK(depg0213_epd_window(epd, epd->direction, x_start, x_end, y_start, y_end));

    uint8_t command[5] = { 0x4E, 0x0C - x_start / 8, 0x4F, 0xD3 - y_start, 0x00 };

    switch(epd->direction) {
        case DEPG0213_VERTICAL_INVERSE:
        // X->0x00
        command[1] = x_start / 8;

        // Y->0x00
        command[3] = y_start;
        break;
        case DEPG0213_HORIZONTAL: // X-Y inversed
        // X->0x00
        command[1] = y_start / 8;

        // Y->0xD3
        command[3] = 0xD3 - x_start;
        break;
        case DEPG0213_HORIZONTAL_INVERSE: // X-Y inversed
        // X->0x0C
        command[1] = 0x0C - y_start / 8;

        // Y-> 0x00
        command[3] = x_start;
        break;
        case DEPG0213_VERTICAL:
        default:
        break;
    }

    uint16_t transfer_bytes = (y_end - y_start + 1) * (x_end - x_start + 1) / 8;

    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, command, 0x02));
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &command[2], 0x03));
    uint8_t wr_command = 0x24;
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &wr_command, 0x01));
    DEPG0213_ERROR_CHECK(epd->cb.write_data_cb(epd->user_data, bw_image, transfer_bytes));
    wr_command = 0x26;
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, &wr_command, 0x01));
    DEPG0213_ERROR_CHECK(epd->cb.write_data_cb(epd->user_data, red_image, transfer_bytes));

    return DEPG0213_OK;
}

depg0213_ret_t depg0213_epd_deepsleep(depg0213_epd_t *epd) {
    if(epd->deep_sleep) return DEPG0213_OK;

    uint8_t deepsleep_command[2] = { 0x10, 0x01 };
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, deepsleep_command, 0x02));

    epd->deep_sleep = 1;

    return DEPG0213_OK;
}

depg0213_ret_t depg0213_epd_window(depg0213_epd_t *epd, depg0213_direction_t direction,
                                   uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end) {

    if(epd->deep_sleep) {
        DEPG0213_ERROR_CHECK(depg0213_epd_init(epd));
    }

    // Sanity checks
    if(x_end < x_start || y_end < y_start) return DEPG0213_ERROR;

    if(direction == DEPG0213_VERTICAL || direction == DEPG0213_VERTICAL_INVERSE) {
        if(x_end > 103 || y_end > 211) {
            return DEPG0213_ERROR;
        }
        if(x_start % 8 != 0) return DEPG0213_ERROR;
        if((x_end + 1) % 8 != 0) return DEPG0213_ERROR;
    }
    else {
        if(x_end > 211 || y_end > 103) {
            return DEPG0213_ERROR;
        }
        if(x_start % 8 != 0) return DEPG0213_ERROR;
        if((y_end + 1) % 8 != 0) return DEPG0213_ERROR;
    }

    // Default mode: VERTICAL, X: 0x0C->0x00, Y: 0xD3->0x00, AM-|Y-|X-
    uint8_t cmd_ram_x_address[] = { 0x44, 0x0C - x_start / 8, 0x0C - x_end / 8 }; // CMD, START, END
    uint8_t cmd_ram_y_address[] = { 0x45, 0xD3 - y_start, 0x00, 0xD3 - y_end, 0x00 }; // CMD, START_L, START_H, END_L, END_H
    uint8_t cmd_data_entry[] = { 0x11, 0x00 }; // CMD, SCAN_X->DESC, SCAN_Y->DESC, AM->0 [2:0]=AM|Y+|X+

    switch(direction) {
        case DEPG0213_VERTICAL_INVERSE:
        // START: 0x00, END: 0x0C
        cmd_ram_x_address[1] = x_start / 8;
        cmd_ram_x_address[2] = x_end / 8;

        // START: 0x00, END: 0xD3
        cmd_ram_y_address[1] = y_start;
        cmd_ram_y_address[3] = y_end;

        // AM-|Y+|X+
        cmd_data_entry[1] = 0x03;
        break;
        case DEPG0213_HORIZONTAL:
        // START: 0x00, END: 0x0C
        cmd_ram_x_address[1] = y_start / 8;
        cmd_ram_x_address[2] = y_end / 8;

        // NO CHANGE FOR Y AXIS
        cmd_ram_y_address[1] = 0xD3 - x_start;
        cmd_ram_y_address[3] = 0xD3 - x_end;

        // AM+|Y-|X+
        cmd_data_entry[1] = 0x05;
        break;
        case DEPG0213_HORIZONTAL_INVERSE:
        // NO CHANGE FOR X AXIS
        cmd_ram_x_address[1] = 0x0C - y_start / 8;
        cmd_ram_x_address[2] = 0x0C - y_end / 8;

        // START: 0x00, END: 0xD3
        cmd_ram_y_address[1] = x_start;
        cmd_ram_y_address[3] = x_end;

        // AM+|Y+|X-
        cmd_data_entry[1] = 0x06;
        break;
        case DEPG0213_VERTICAL:
        default:
        break;
    }

    // Write mode to screen.
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, cmd_ram_x_address, 0x03));
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, cmd_ram_y_address, 0x05));
    DEPG0213_ERROR_CHECK(epd->cb.write_cmd_cb(epd->user_data, cmd_data_entry, 0x02));

    epd->direction = direction;

    return DEPG0213_OK;
}