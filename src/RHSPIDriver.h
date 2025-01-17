// RHSPIDriver.h
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2014 Mike McCauley
// $Id: RHSPIDriver.h,v 1.13 2018/02/11 23:57:18 mikem Exp $

#ifndef RHSPIDriver_h
#define RHSPIDriver_h

#include <RHGenericDriver.h>
#include <RHHardwareSPI.h>

// This is the bit in the SPI address that marks it as a write
#define RH_SPI_WRITE_MASK 0x80

#if (RH_PLATFORM == RH_PLATFORM_RASPI)
#define RPI_CE0_CE1_FIX { \
          if (_slaveSelectPin!=7) {   \
            bcm2835_gpio_fsel(7,BCM2835_GPIO_FSEL_OUTP); \
            bcm2835_gpio_write(7,HIGH); \
          }                           \
          if (_slaveSelectPin!=8) {   \
            bcm2835_gpio_fsel(8,BCM2835_GPIO_FSEL_OUTP); \
            bcm2835_gpio_write(8,HIGH); \
          }                           \
        }
#else
#define RPI_CE0_CE1_FIX {}
#endif


class RHGenericSPI;

/////////////////////////////////////////////////////////////////////
/// \class RHSPIDriver RHSPIDriver.h <RHSPIDriver.h>
/// \brief Base class for RadioHead drivers that use the SPI bus
/// to communicate with its transport hardware.
///
/// This class can be subclassed by Drivers that require to use the SPI bus.
/// It can be configured to use either the RHHardwareSPI class (if there is one available on the platform)
/// of the bitbanged RHSoftwareSPI class. The default behaviour is to use a pre-instantiated built-in RHHardwareSPI
/// interface.
///
/// SPI bus access is protected by ATOMIC_BLOCK_START and ATOMIC_BLOCK_END, which will ensure interrupts 
/// are disabled during access.
/// 
/// The read and write routines implement commonly used SPI conventions: specifically that the MSB
/// of the first byte transmitted indicates that it is a write and the remaining bits indicate the rehgister to access)
/// This can be overriden 
/// in subclasses if necessaryor an alternative class, RHNRFSPIDriver can be used to access devices like 
/// Nordic NRF series radios, which have different requirements.
///
/// Application developers are not expected to instantiate this class directly: 
/// it is for the use of Driver developers.
class RHSPIDriver : public RHGenericDriver
{
public:
    /// Constructor
    /// \param[in] slaveSelectPin The controler pin to use to select the desired SPI device. This pin will be driven LOW
    /// during SPI communications with the SPI device that uis iused by this Driver.
    /// \param[in] spi Reference to the SPI interface to use. The default is to use a default built-in Hardware interface.
    RHSPIDriver(uint8_t slaveSelectPin = SS, RHGenericSPI& spi = hardware_spi);

    /// Initialise the Driver transport hardware and software.
    /// Make sure the Driver is properly configured before calling init().
    /// \return true if initialisation succeeded.
    bool init();

    /// Reads a single register from the SPI device
    /// \param[in] reg Register number
    /// \return The value of the register
    uint8_t        spiRead(uint8_t reg);

    /// Writes a single byte to the SPI device
    /// \param[in] reg Register number
    /// \param[in] val The value to write
    /// \return Some devices return a status byte during the first data transfer. This byte is returned.
    ///  it may or may not be meaningfule depending on the the type of device being accessed.
    uint8_t           spiWrite(uint8_t reg, uint8_t val);

    /// Reads a number of consecutive registers from the SPI device using burst read mode
    /// \param[in] reg Register number of the first register
    /// \param[in] dest Array to write the register values to. Must be at least len bytes
    /// \param[in] len Number of bytes to read
    /// \return Some devices return a status byte during the first data transfer. This byte is returned.
    ///  it may or may not be meaningfule depending on the the type of device being accessed.
    uint8_t           spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len);

    /// Write a number of consecutive registers using burst write mode
    /// \param[in] reg Register number of the first register
    /// \param[in] src Array of new register values to write. Must be at least len bytes
    /// \param[in] len Number of bytes to write
    /// \return Some devices return a status byte during the first data transfer. This byte is returned.
    ///  it may or may not be meaningfule depending on the the type of device being accessed.
    uint8_t           spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len);

    /// Set or change the pin to be used for SPI slave select.
    /// This can be called at any time to change the
    /// pin that will be used for slave select in subsquent SPI operations.
    /// \param[in] slaveSelectPin The pin to use
    void setSlaveSelectPin(uint8_t slaveSelectPin);

    /// Set the SPI interrupt number
    /// If SPI transactions can occur within an interrupt, tell the low level SPI
    /// interface which interrupt is used
    /// \param[in] interruptNumber the interrupt number
    void spiUsingInterrupt(uint8_t interruptNumber);

    protected:
    /// Reference to the RHGenericSPI instance to use to transfer data with the SPI device
    RHGenericSPI&       _spi;

    /// The pin number of the Slave Select pin that is used to select the desired device.
    uint8_t             _slaveSelectPin;
    uint8_t             _interuptPin; // If interrupts are used else NOT_AN_INTERRUPT
};

#endif
