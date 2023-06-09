/*************************************************************************
 * File Name: ota7290b.c
 * Author: Mark Zhu
 * Mail:   zhufanghua@imilab.com
 * Created Time: 2022年01月06日 星期四 16时15分59秒
 ************************************************************************/

#include <config.h>
#include <serial.h>
#include <common.h>
#include <lcd.h>
#include <linux/list.h>
#include <linux/fb.h>
#include <asm/types.h>
#include <asm/arch/tcu.h>
#include <asm/arch/gpio.h>
#include <regulator.h>

#include <jz_lcd/jz_dsim.h>
#include <jz_lcd/ota7290b.h>
#include <jz_lcd/jz_lcd_v14.h>
#include "../jz_mipi_dsi/jz_mipi_dsih_hal.h"


#define PWM_EN GPIO_PC(3)

vidinfo_t panel_info = { 600, 1024, LCD_BPP, };

struct auo_ota7290b_platform_data auo_ota7290b_pdata;

struct fb_videomode jzfb1_videomode = {
	.name = "ota7290b",
	.refresh = 60,
	.xres = 600,
	.yres = 1024,
	.pixclock = KHZ2PICOS(53356),
	.left_margin = 180,
	.right_margin = 80,
	.upper_margin = 25,
	.lower_margin = 50,
	.hsync_len = 24,
	.vsync_len = 10,
	.sync = FB_SYNC_HOR_HIGH_ACT & FB_SYNC_VERT_HIGH_ACT,
	.vmode = FB_VMODE_NONINTERLACED,
	.flag = 0,
};

struct jzfb_tft_config auo_ota7290b_cfg = {
	.pix_clk_inv = 0,
	.de_dl = 0,
	.sync_dl = 0,
	.color_even = TFT_LCD_COLOR_EVEN_RGB,
	.color_odd = TFT_LCD_COLOR_ODD_RGB,
	.mode = TFT_LCD_MODE_PARALLEL_888,
};

struct jzfb_config_info jzfb1_init_data = {
	.modes = &jzfb1_videomode,
	.lcd_type = LCD_TYPE_MIPI_TFT,
	.bpp = 24,

	.tft_config = &auo_ota7290b_cfg,
	.pixclk_falling_edge = 0,
	.date_enable_active_low = 0,

	.dither_enable          = 0,
	.dither.dither_red      = 0,
	.dither.dither_green    = 0,
	.dither.dither_blue     = 0,
};

struct dsi_config jz_dsi_config = {
	.max_lanes = 4,
	.max_hs_to_lp_cycles = 100,
	.max_lp_to_hs_cycles = 40,
	.max_bta_cycles = 4095,
	.color_mode_polarity = 1,
	.shut_down_polarity = 1,
};

struct video_config jz_dsi_video_config={
	.no_of_lanes = 4,
	.virtual_channel =0,
	.color_coding = COLOR_CODE_24BIT,
	.video_mode = VIDEO_BURST_WITH_SYNC_PULSES,
	.receive_ack_packets = 0,
	.is_18_loosely = 0,
	.data_en_polarity = 1,
	.byte_clock = 0,
	.byte_clock_coef = MIPI_PHY_BYTE_CLK_COEF_MUL6_DIV5,
};

struct dsi_device jz_dsi = {
	.dsi_config = &jz_dsi_config,
	.video_config = &jz_dsi_video_config,
	.data_lane = 4,
	.bpp_info = 24,
	.max_bps =1000,
};

struct auo_ota7290b_platform_data auo_ota7290b_pdata = {
};

void auo_ota7290b_regulator_enable(struct dsi_device *dsi)
{
}

void auo_ota7290b_regulator_disable(struct dsi_device *dsi)
{
}

void auo_ota7290b_sleep_in(struct dsi_device *dsi) /* enter sleep */
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x10, 0x00};
	write_command(dsi, data_to_send);
}

void auo_ota7290b_sleep_out(struct dsi_device *dsi) /* exit sleep */
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x11, 0x00};
	write_command(dsi, data_to_send);
}

void auo_ota7290b_display_on(struct dsi_device *dsi) /* display on */
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x29, 0x00};
	write_command(dsi, data_to_send);
}

void auo_ota7290b_display_off(struct dsi_device *dsi) /* display off */
{
	struct dsi_cmd_packet data_to_send = {0x05, 0x28, 0x00};
	write_command(dsi, data_to_send);
}

void auo_ota7290b_set_pixel_off(struct dsi_device *dsi) /* set_pixels_off */
{
	struct dsi_cmd_packet data_to_send = {0x39, 0x02, 0x00, {0x22,0x00}};
	write_command(dsi, data_to_send);
}

void auo_ota7290b_set_pixel_on(struct dsi_device *dsi) /* set_pixels_on */
{
	struct dsi_cmd_packet data_to_send = {0x39, 0x02, 0x00, {0x23,0x00}};
	write_command(dsi, data_to_send);
}

void auo_ota7290b_set_brightness(struct dsi_device *dsi, unsigned int brightness) /* set brightness */
{
	if(brightness >= 255) {
		debug("the max brightness is 255, set it 255\n");
		brightness = 255;
	}
	struct dsi_cmd_packet data_to_send = {0x39, 0x02, 0x00, {0x51, brightness}};
	write_command(dsi, data_to_send);
}

void panel_power_on(void)
{
	gpio_direction_output(PWM_EN, 1);
	serial_puts("auo_ota7290b panel display on\n");
}

void panel_power_off(void)
{
	serial_puts("lcd_auo_ota7290b panel display off\n");
}

void panel_init_sequence(struct dsi_device *dsi)
{
	printf("%s %s ###########\r\n",__FILE__,__func__);
}

struct dsi_cmd_packet auo_ota7290b_cmd_list1[] = {
    {0x15, 0xB0, 0x5A},
    {0x15, 0xB1, 0x00},
    {0x15, 0x89, 0x01},
    {0x15, 0x91, 0x17},//16 BIST
    {0x15, 0xB1, 0x03},
    {0x15, 0x2C, 0x28},
    {0x15, 0x00, 0xDF},
    {0x15, 0x01, 0xEF},
    {0x15, 0x02, 0xF7},
    {0x15, 0x03, 0xFB},
    {0x15, 0x04, 0xFD},
    {0x15, 0x05, 0x00},
    {0x15, 0x06, 0x00},
    {0x15, 0x07, 0x00},
    {0x15, 0x08, 0x00},
    {0x15, 0x09, 0x00},
    {0x15, 0x0A, 0x01},
    {0x15, 0x0B, 0x00},
    {0x15, 0x0C, 0x00},
    {0x15, 0x0D, 0x00},
    {0x15, 0x0E, 0x24},
    {0x15, 0x0F, 0x1C},
    {0x15, 0x10, 0xC9},
    {0x15, 0x11, 0x60},
    {0x15, 0x12, 0x70},
    {0x15, 0x13, 0x01},
    {0x15, 0x14, 0xE3},//E2:3LANE E1:2LANE
    {0x15, 0x15, 0xFF},
    {0x15, 0x16, 0x3D},
    {0x15, 0x17, 0x0E},
    {0x15, 0x18, 0x01},
    {0x15, 0x19, 0x00},
    {0x15, 0x1A, 0x00},
    {0x15, 0x1B, 0xFC},
    {0x15, 0x1C, 0x0B},
    {0x15, 0x1D, 0xA0},
    {0x15, 0x1E, 0x03},
    {0x15, 0x1F, 0x04},
    {0x15, 0x20, 0x0C},
    {0x15, 0x21, 0x00},
    {0x15, 0x22, 0x04},
    {0x15, 0x23, 0x81},
    {0x15, 0x24, 0x1F},
    {0x15, 0x25, 0x10},
    {0x15, 0x26, 0x9B},
    {0x15, 0x2C, 0x2C},
    {0x15, 0x2D, 0x01},
    {0x15, 0x2E, 0x84},
    {0x15, 0x2F, 0x00},
    {0x15, 0x30, 0x02},
    {0x15, 0x31, 0x08},
    {0x15, 0x32, 0x01},
    {0x15, 0x33, 0x1C},
    {0x15, 0x34, 0x70},
    {0x15, 0x35, 0xFF},
    {0x15, 0x36, 0xFF},
    {0x15, 0x37, 0xFF},
    {0x15, 0x38, 0xFF},
    {0x15, 0x39, 0xFF},
    {0x15, 0x3A, 0x05},
    {0x15, 0x3B, 0x00},
    {0x15, 0x3C, 0x00},
    {0x15, 0x3D, 0x00},
    {0x15, 0x3E, 0x0F},
    {0x15, 0x3F, 0x84},
    {0x15, 0x40, 0x2A},
    {0x15, 0x41, 0x00},
    {0x15, 0x42, 0x01},
    {0x15, 0x43, 0x40},
    {0x15, 0x44, 0x05},
    {0x15, 0x45, 0xE8},
    {0x15, 0x46, 0x16},
    {0x15, 0x47, 0x00},
    {0x15, 0x48, 0x00},
    {0x15, 0x49, 0x88},
    {0x15, 0x4A, 0x08},
    {0x15, 0x4B, 0x05},
    {0x15, 0x4C, 0x03},
    {0x15, 0x4D, 0xD0},
    {0x15, 0x4E, 0x13},
    {0x15, 0x4F, 0xFF},
    {0x15, 0x50, 0x0A},
    {0x15, 0x51, 0x53},
    {0x15, 0x52, 0x26},
    {0x15, 0x53, 0x22},
    {0x15, 0x54, 0x09},
    {0x15, 0x55, 0x22},
    {0x15, 0x56, 0x00},
    {0x15, 0x57, 0x1C},
    {0x15, 0x58, 0x03},
    {0x15, 0x59, 0x3F},
    {0x15, 0x5A, 0x28},
    {0x15, 0x5B, 0x01},
    {0x15, 0x5C, 0xCC},//GIP 设定
    {0x15, 0x5D, 0x21},
    {0x15, 0x5E, 0x04},
    {0x15, 0x5F, 0x13},
    {0x15, 0x60, 0x42},
    {0x15, 0x61, 0x08},
    {0x15, 0x62, 0x64},
    {0x15, 0x63, 0xEB},
    {0x15, 0x64, 0x10},
    {0x15, 0x65, 0xA8},
    {0x15, 0x66, 0x84},
    {0x15, 0x67, 0x8E},
    {0x15, 0x68, 0x29},
    {0x15, 0x69, 0x11},
    {0x15, 0x6A, 0x42},
    {0x15, 0x6B, 0x38},
    {0x15, 0x6C, 0x21},
    {0x15, 0x6D, 0x84},
    {0x15, 0x6E, 0x50},
    {0x15, 0x6F, 0xB6},
    {0x15, 0x70, 0x0E},
    {0x15, 0x71, 0xA1},
    {0x15, 0x72, 0xCE},
    {0x15, 0x73, 0xF8},
    {0x15, 0x74, 0xDA},
    {0x15, 0x75, 0x1A},//GIP 设定
    {0x15, 0x76, 0x80},
    {0x15, 0x77, 0x00},
    {0x15, 0x78, 0x5F},
    {0x15, 0x79, 0x00},
    {0x15, 0x7A, 0x00},
    {0x15, 0x7B, 0x00},
    {0x15, 0x7C, 0x00},
    {0x15, 0x7D, 0x00},
    {0x15, 0x7E, 0x00},
    {0x15, 0x7F, 0xFE},
    {0x15, 0xB1, 0x02},
    {0x15, 0x00, 0xFF},
    {0x15, 0x01, 0x01},
    {0x15, 0x02, 0x00},
    {0x15, 0x03, 0x00},
    {0x15, 0x04, 0x00},
    {0x15, 0x05, 0x00},
    {0x15, 0x06, 0x00},
    {0x15, 0x07, 0x00},
    {0x15, 0x08, 0xC0},
    {0x15, 0x09, 0x00},
    {0x15, 0x0A, 0x00},
    {0x15, 0x0B, 0x00},
    {0x15, 0x0C, 0xE6},
    {0x15, 0x0D, 0x0D},
    {0x15, 0x0F, 0x00},
    {0x15, 0x10, 0xFF},//gamma
    {0x15, 0x11, 0xEC},
    {0x15, 0x12, 0xE7},
    {0x15, 0x13, 0xB7},
    {0x15, 0x14, 0x83},
    {0x15, 0x15, 0x18},
    {0x15, 0x16, 0x06},
    {0x15, 0x17, 0xDA},
    {0x15, 0x18, 0xF9},
    {0x15, 0x19, 0xFD},
    {0x15, 0x1A, 0xFB},
    {0x15, 0x1B, 0x0F},//gamma
    {0x15, 0x1C, 0xFF},
    {0x15, 0x1D, 0xFF},
    {0x15, 0x1E, 0xFF},
    {0x15, 0x1F, 0xFF},
    {0x15, 0x20, 0xFF},
    {0x15, 0x21, 0xFF},
    {0x15, 0x22, 0xFF},
    {0x15, 0x23, 0xFF},
    {0x15, 0x24, 0xFF},
    {0x15, 0x25, 0xFF},
    {0x15, 0x26, 0xFF},
    {0x15, 0x27, 0x1F},
    {0x15, 0x28, 0xFF},//VCOM设定
    {0x15, 0x29, 0xFF},
    {0x15, 0x2A, 0xFF},
    {0x15, 0x2B, 0xFF},
    {0x15, 0x2C, 0xFF},
    {0x15, 0x2D, 0x07},
    {0x15, 0x2F, 0xFF},
    {0x15, 0x30, 0x7F},
    {0x15, 0x31, 0x91},
    {0x15, 0x32, 0x00},
    {0x15, 0x33, 0x0C},
    {0x15, 0x35, 0x7F},
    {0x15, 0x36, 0x0C},
    {0x15, 0x38, 0x7F},
    {0x15, 0x3A, 0x80},
    {0x15, 0x3B, 0x01},
    {0x15, 0x3C, 0xC0},
    {0x15, 0x3D, 0x32},
    {0x15, 0x3E, 0x00},
    {0x15, 0x3F, 0x58},
    {0x15, 0x40, 0x06},
    {0x15, 0x41, 0x00},
    {0x15, 0x42, 0xCB},
    {0x15, 0x43, 0x00},
    {0x15, 0x44, 0x60},
    {0x15, 0x45, 0x09},
    {0x15, 0x46, 0x00},
    {0x15, 0x47, 0x00},
    {0x15, 0x48, 0x8B},
    {0x15, 0x49, 0xD2},
    {0x15, 0x4A, 0x01},
    {0x15, 0x4B, 0x00},
    {0x15, 0x4C, 0x10},
    {0x15, 0x4D, 0x40},
    {0x15, 0x4E, 0x0D},
    {0x15, 0x4F, 0x61},
    {0x15, 0x50, 0x3C},
    {0x15, 0x51, 0x7A},
    {0x15, 0x52, 0x34},
    {0x15, 0x53, 0x99},
    {0x15, 0x54, 0xA2},
    {0x15, 0x55, 0x03},
    {0x15, 0x56, 0x6C},
    {0x15, 0x57, 0x1A},
    {0x15, 0x58, 0x05},
    {0x15, 0x59, 0xF0},
    {0x15, 0x5A, 0xFB},
    {0x15, 0x5B, 0xFD},
    {0x15, 0x5C, 0x7E},
    {0x15, 0x5D, 0xBF},
    {0x15, 0x5E, 0x1F},
    {0x15, 0x5F, 0x00},
    {0x15, 0x60, 0xF0},
    {0x15, 0x61, 0xF3},
    {0x15, 0x62, 0xFB},
    {0x15, 0x63, 0xF9},
    {0x15, 0x64, 0xFD},
    {0x15, 0x65, 0x7E},
    {0x15, 0x66, 0x00},
    {0x15, 0x67, 0x00},
    {0x15, 0x68, 0x14},
    {0x15, 0x69, 0x89},
    {0x15, 0x6A, 0x70},
    {0x15, 0x6B, 0xFC},
    {0x15, 0x6C, 0xFC},
    {0x15, 0x6D, 0xFC},
    {0x15, 0x6E, 0xFC},
    {0x15, 0x6F, 0xFC},
    {0x15, 0x70, 0x7E},
    {0x15, 0x71, 0xBF},
    {0x15, 0x72, 0xDF},
    {0x15, 0x73, 0xCF},
    {0x15, 0x74, 0xCF},
    {0x15, 0x75, 0xCF},
    {0x15, 0x76, 0x0F},
    {0x15, 0x77, 0x00},
    {0x15, 0x78, 0x00},
    {0x15, 0x79, 0x00},
    {0x15, 0x7A, 0x7E},
    {0x15, 0x7B, 0x7E},
    {0x15, 0x7C, 0x7E},
    {0x15, 0x7D, 0x7E},
    {0x15, 0x7E, 0x7E},
    {0x15, 0x7F, 0xBF},
    {0x15, 0x0B, 0x04},
    {0x15, 0xB1, 0x03},
    {0x15, 0x2C, 0x2C},
    {0x15, 0xB1, 0x00},
    {0x15, 0x89, 0x03},
};

static void auo_ota7290b_panel_condition_setting(struct dsi_device *dsi)
{
    int  i;
	printf("zhufanghua [%s---%d] init reg \n", __func__, __LINE__);
#if 0
	for(i = 0; i < ARRAY_SIZE(auo_ota7290b_cmd_list1); i++) {
		write_command(dsi,  auo_ota7290b_cmd_list1[i]);
	}
#endif
}
void panel_init_set_sequence(struct dsi_device *dsi)
{
	printf("xxx \n");
	auo_ota7290b_panel_condition_setting(dsi);
	auo_ota7290b_sleep_out(dsi);
	mdelay(350);
}
void panel_display_on(struct dsi_device *dsi)
{
	auo_ota7290b_display_on(dsi);
}

void panel_pin_init(void)
{
	printf("[%s---%d] zhufanghua en gpio = %d\n", __func__, __LINE__, PWM_EN);
	int ret = gpio_request(PWM_EN, "lcd pwn en");
	serial_puts("auo_ota7290b panel display pin init\n");
}


