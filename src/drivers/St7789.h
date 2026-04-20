#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

#include <FreeRTOS.h>

namespace Pinetime {
  namespace Drivers {
    class Spi;

    class St7789 {
    public:
      explicit St7789(Spi& spi, uint8_t pinDataCommand, uint8_t pinReset);
      St7789(const St7789&) = delete;
      St7789& operator=(const St7789&) = delete;
      St7789(St7789&&) = delete;
      St7789& operator=(St7789&&) = delete;

      void Init();
      void Uninit();

      void VerticalScrollStartAddress(uint16_t line);

      void DrawBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* data, size_t size);

      void LowPowerOn();
      void LowPowerOff();
      void Sleep();
      void Wakeup();

      // Set the Memory Data Access Control (MADCTL) register directly.
      // Use this to change display orientation at runtime.
      // Restore by calling with the original value when done.
      void SetOrientation(uint8_t madctl);

    private:
      Spi& spi;
      uint8_t pinDataCommand;
      uint8_t pinReset;
      uint8_t verticalScrollingStartAddress = 0;
      bool sleepIn;

      // Keep the rest of the private interface exactly as-is
      enum class Commands : uint8_t {
        SoftwareReset = 0x01,
        SleepIn = 0x10,
        SleepOut = 0x11,
        NormalModeOn = 0x13,
        DisplayInversionOn = 0x21,
        DisplayInversionOff = 0x20,
        DisplayOff = 0x28,
        DisplayOn = 0x29,
        ColumnAddressSet = 0x2a,
        RowAddressSet = 0x2b,
        WriteToRam = 0x2c,
        MemoryDataAccessControl = 0x36,
        VerticalScrollStartAddress = 0x37,
        IdleModeOff = 0x38,
        IdleModeOn = 0x39,
        PixelFormat = 0x3a,
        Command2Enable = 0xdf,
        Porch = 0xb2,
        GateControl = 0xb7,
        FrameRateNormal = 0xbb,
        FrameRateIdle = 0xc2,
        PowerControl1 = 0xd0,
        PowerControl2 = 0xd6,
        VdvSet = 0xc4,
      };

      TickType_t lastSleepExit = 0;
      uint8_t addrWindowArgs[4] = {};
      uint8_t verticalScrollArgs[2] = {};

      void HardwareReset();
      void SoftwareReset();
      void Command2Enable();
      void SleepOut();
      void SleepIn();
      void EnsureSleepOutPostDelay();
      void PixelFormat();
      void MemoryDataAccessControl();
      void DisplayInversionOn();
      void NormalModeOn();
      void IdleModeOn();
      void IdleModeOff();
      void PorchSet();
      void FrameRateNormalSet();
      void IdleFrameRateOn();
      void IdleFrameRateOff();
      void DisplayOn();
      void DisplayOff();
      void PowerControl();
      void GateControl();
      void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
      void WriteToRam(const uint8_t* data, size_t size);
      void SetVdv();

      void WriteData(uint8_t data);
      void WriteData(const uint8_t* data, size_t size);
      void WriteCommand(uint8_t data);
      void WriteCommand(const uint8_t* data, size_t size);
      void WriteSpi(const uint8_t* data, size_t size, const std::function<void()>& preTransactionHook);
    };
  }
}
