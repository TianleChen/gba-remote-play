#ifndef PROTOCOL_H
#define PROTOCOL_H

#define RENDER_WIDTH 120
#define RENDER_HEIGHT 80
#define RENDER_SCALE 2
#define PALETTE_COLORS 256
#define PACKET_SIZE 4
#define COLOR_SIZE 2
#define PIXEL_SIZE 1
#define QUANTIZER_SPEED 8
#define SPI_MODE 3
#define SPI_FREQUENCY 1600000
#define SPI_DELAY_MICROSECONDS 7
#define MAX_BLIND_FRAMES 5
#define TOTAL_PIXELS (RENDER_WIDTH * RENDER_HEIGHT)
#define COLORS_PER_PACKET (PACKET_SIZE / COLOR_SIZE)
#define PIXELS_PER_PACKET (PACKET_SIZE / PIXEL_SIZE)
#define TEMPORAL_DIFF_SIZE (TOTAL_PIXELS / 8)

#define FIRST_COLOR(PACKET) (PACKET & 0xffff)
#define SECOND_COLOR(PACKET) ((PACKET >> 16) & 0xffff)

#define CMD_RESET 0x98765400

#define CMD_FRAME_START_RPI 0x98765411
#define CMD_FRAME_START_GBA 0x98765412

#define CMD_PALETTE_START_RPI 0x98765421
#define CMD_PALETTE_START_GBA 0x98765422

#define CMD_PIXELS_START_RPI 0x98765431
#define CMD_PIXELS_START_GBA 0x98765432

#define CMD_FRAME_END_RPI 0x98765441
#define CMD_FRAME_END_GBA 0x98765442

#endif  // PROTOCOL_H
