
#ifndef AM65X_H
#define AM65X_H

#include <stdint.h>

#define AM65X_VERSION							10000		/* Version 1.00 */



typedef enum
{
    AM65X_I2C_REASON_OK   						= 0x00,      /*!< Success */
    AM65X_I2C_REASON_ERROR_NACK					= 0x01,      /*!< Received a NACK */
    AM65X_I2C_REASON_ERROR_CLKT					= 0x02,      /*!< Received Clock Stretch Timeout */
    AM65X_I2C_REASON_ERROR_DATA					= 0x04       /*!< Not all data is sent / received */
} AM65XI2CReasonCodes;



#ifdef __cplusplus
extern "C" {
#endif

/*! Bases of the I2C registers.
  Available after am65x_init has been called (as root)
*/
extern volatile uint32_t *am65x_I2C0;
extern volatile uint32_t *am65x_I2C1;

#define am65x_I2C_DIV							0x0014		/*!< I2C Master Clock Divider */		
#define am65x_I2C_A								0x000c		/*!< I2C Master Slave Address */
#define am65x_I2C0_BASE							0x205000	/*! Base Address of the I2C0 registers */
#define	am65x_I2C1_BASE							0x804000	/*! Base Address of the I2C1 registers */
#define	am65x_I2C_S_CLKT						0x001c		/*!< I2C Master Clock Stretch Timeout */
#define	am65x_I2C_S_ERR							0x00000100	/*!< ACK error */
#define	am65x_I2C_S_DONE						0x00000002	/*!< Transfer DONE */
#define	am65x_I2C_C_CLEAR_1						0x00000020	/*!< Clear FIFO Clear */
#define	am65x_I2C_C	 		 					0x0000		/*!< I2C Master Control */
#define am65x_I2C_S 							0x0004		/*!< I2C Master Status */
#define	am65x_I2C_FIFO							0x0010		/*!< I2C Master Data FIFO */
#define	am65x_I2C_DLEN							0x0008		/*!< I2C Master Data Length */
#define	am65x_I2C_S_TXD							0x00000010	/*!< TXD FIFO can accept data */
#define	am65x_I2C_FIFO_SIZE						16			/*!< I2C FIFO size */

    extern int am65x_init(void);

    extern int am65x_close(void);

    extern unsigned int  am65x_version(void);

    /*! @} */

    /*! \defgroup lowlevel Low level register access
      These functions provide low level register access, and should not generally
      need to be used 
       
      @{
    */

    /*! Reads 32 bit value from a peripheral address WITH a memory barrier before and after each read.
      This is safe, but slow.  The MB before protects this read from any in-flight reads that didn't
      use a MB.  The MB after protects subsequent reads from another peripheral.

      \param[in] paddr Physical address to read from. See  am65x_GPIO_BASE etc.
      \return the value read from the 32 bit register
      \sa Physical Addresses
    */
    extern uint32_t am65x_peri_read(volatile uint32_t* paddr);


    /*! Writes 32 bit value from a peripheral address WITH a memory barrier before and after each write
      This is safe, but slow.  The MB before ensures that any in-flight write to another peripheral
      completes before this write is issued.  The MB after ensures that subsequent reads and writes
      to another peripheral will see the effect of this write.

      This is a tricky optimization; if you aren't sure, use the barrier version.

      \param[in] paddr Physical address to read from. See  am65x_GPIO_BASE etc.
      \param[in] value The 32 bit value to write
      \sa Physical Addresses
    */
    extern void  am65x_peri_write(volatile uint32_t* paddr, uint32_t value);


    /*! Alters a number of bits in a 32 peripheral regsiter.
      It reads the current valu and then alters the bits defines as 1 in mask, 
      according to the bit value in value. 
      All other bits that are 0 in the mask are unaffected.
      Use this to alter a subset of the bits in a register.
      Memory barriers are used.  Note that this is not atomic; an interrupt
      routine can cause unexpected results.
      \param[in] paddr Physical address to read from. See  am65x_GPIO_BASE etc.
      \param[in] value The 32 bit value to write, masked in by mask.
      \param[in] mask Bitmask that defines the bits that will be altered in the register.
      \sa Physical Addresses
    */
    extern void  am65x_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask);

    
    /*! @} */

    /*! \defgroup i2c I2C access
      These functions let you use I2C (The Broadcom Serial Control bus with the Philips
      I2C bus/interface version 2.1 January 2000.) to interface with an external I2C device.
      @{
    */

    /*! Start I2C operations.
      Forces RPi I2C pins P1-03 (SDA) and P1-05 (SCL)
      to alternate function ALT0, which enables those pins for I2C interface.
      You should call  am65x_i2c_end() when all I2C functions are complete to return the pins to
      their default functions
      \return 1 if successful, 0 otherwise (perhaps because you are not running as root)
      \sa   am65x_i2c_end()
    */
    extern int  am65x_i2c_begin(void);

    /*! End I2C operations.
      I2C pins P1-03 (SDA) and P1-05 (SCL)
      are returned to their default INPUT behaviour.
    */
    extern void  am65x_i2c_end(void);

    /*! Sets the I2C slave address.
      \param[in] addr The I2C slave address.
    */
    extern void  am65x_i2c_setSlaveAddress(uint8_t addr);

    /*! Sets the I2C clock divider and therefore the I2C clock speed.
      \param[in] divider The desired I2C clock divider, one of  am65x_I2C_CLOCK_DIVIDER_*,
      see \ref  am65xI2CClockDivider
    */
    extern void  am65x_i2c_setClockDivider(uint16_t divider);

    /*! Sets the I2C clock divider by converting the baudrate parameter to
      the equivalent I2C clock divider. ( see \sa  am65x_i2c_setClockDivider)
      For the I2C standard 100khz you would set baudrate to 100000
      The use of baudrate corresponds to its use in the I2C kernel device
      driver. (Of course,  am65x has nothing to do with the kernel driver)
    */
    extern void  am65x_i2c_set_baudrate(uint32_t baudrate);

    /*! Transfers any number of bytes to the currently selected I2C slave.
      (as previously set by \sa  am65x_i2c_setSlaveAddress)
      \param[in] buf Buffer of bytes to send.
      \param[in] len Number of bytes in the buf buffer, and the number of bytes to send.
      \return reason see \ref  am65xI2CReasonCodes
    */
    extern uint8_t  am65x_i2c_write(const char * buf, uint32_t len);

    /*! Transfers any number of bytes from the currently selected I2C slave.
      (as previously set by \sa  am65x_i2c_setSlaveAddress)
      \param[in] buf Buffer of bytes to receive.
      \param[in] len Number of bytes in the buf buffer, and the number of bytes to received.
      \return reason see \ref  am65xI2CReasonCodes
    */
    extern uint8_t  am65x_i2c_read(char* buf, uint32_t len);
    /*! @}  */
#ifdef __cplusplus
}
#endif

#endif /*  am65x_H */
