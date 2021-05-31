#ifndef GBA_REMOTE_PLAY_H
#define GBA_REMOTE_PLAY_H

#include "Config.h"
#include "FrameBuffer.h"
#include "ImageQuantizer.h"
#include "Protocol.h"
#include "SPIMaster.h"
#include "TemporalDiffBitArray.h"

class GBARemotePlay {
 public:
  GBARemotePlay() {
    spiMaster = new SPIMaster(SPI_MODE, SPI_FREQUENCY, SPI_DELAY_MICROSECONDS);
    frameBuffer = new FrameBuffer(RENDER_WIDTH, RENDER_HEIGHT);
    imageQuantizer = new ImageQuantizer();
  }

  void run() {
    while (true) {
      if (DEBUG) {
        int ret;
        DEBULOG("Waiting...");
        std::cin >> ret;
        DEBULOG("Sending start command...");
      }

      spiMaster->transfer(CMD_RESET);

      if (!sync(CMD_FRAME_START_RPI, CMD_FRAME_START_GBA))
        continue;

      DEBULOG("Loading frame...");
      uint8_t* rgbaPixels = frameBuffer->loadFrame();
      auto frame = imageQuantizer->quantize(rgbaPixels, RENDER_WIDTH,
                                            RENDER_HEIGHT, QUANTIZER_SPEED);

      if (frame.hasData())
        send(frame);

      lastFrame.clean();
      lastFrame = frame;
    }
  }

  void send(Frame frame) {
    DEBULOG("Calculating diffs...");
    TemporalDiffBitArray diffs;
    diffs.initialize(frame, lastFrame);

    // DEBULOG("Sending diffs...");
    // for (int i = 0; i < TEMPORAL_DIFF_SIZE / PACKET_SIZE; i++)
    //   spiMaster->transfer(((uint32_t*)diffs.data)[i]);

    // DEBULOG("Sending palette command...");
    // if (!sync(CMD_PALETTE_START_RPI, CMD_PALETTE_START_GBA))
    //   return;

    DEBULOG("Sending palette...");
    for (int i = 0; i < PALETTE_COLORS / COLORS_PER_PACKET; i++)
      spiMaster->transfer(((uint32_t*)frame.raw15bppPalette)[i]);

    DEBULOG("Sending pixels command...");
    if (!sync(CMD_PIXELS_START_RPI, CMD_PIXELS_START_GBA))
      return;

    DEBULOG("Sending pixels...");  // TODO: ONLY SEND CHANGED PIXELS
    for (int i = 0; i < frame.totalPixels; i += PIXELS_PER_PACKET)
      spiMaster->transfer(frame.raw8BitPixels[i] |
                          (frame.raw8BitPixels[i + 1] << 8) |
                          (frame.raw8BitPixels[i + 2] << 16) |
                          (frame.raw8BitPixels[i + 3] << 24));

    DEBULOG("Sending end command...");
    if (!sync(CMD_FRAME_END_RPI, CMD_FRAME_END_GBA))
      return;

    DEBULOG("Frame end!");
  }

  ~GBARemotePlay() {
    lastFrame.clean();
    delete spiMaster;
    delete frameBuffer;
    delete imageQuantizer;
  }

 private:
  SPIMaster* spiMaster;
  FrameBuffer* frameBuffer;
  ImageQuantizer* imageQuantizer;
  Frame lastFrame = Frame{0};

  bool sync(uint32_t local, uint32_t remote) {
    uint32_t packet = 0;
    while ((packet = spiMaster->transfer(local)) != remote) {
      if (packet == CMD_RESET) {
        DEBULOG("Reset!");
        return false;
      }
    }

    return true;
  }
};

#endif  // GBA_REMOTE_PLAY_H
