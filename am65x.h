/*    am65xh.h
 *
 *    Copyright 2020/2021 by Martin Shishkov (df6@gulliversoft.com)
 *    This software is distributed under the GNU public license version 2
 *    See also the file 'COPYING'.
 *    Function    :   Hardware underlying interface
 *
 */

#ifndef AM65X_H
#define AM65X_H

#include <stdint.h>

#define AM65X_VERSION							10000		/* Version 1.00 */

#define am65x_CORE_CLK_HZ						1100000

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

    extern int am65x_init(void);

    extern int am65x_close(void);

    extern unsigned int  am65x_version(void);
    
    /*! @} */

    /*! \defgroup i2c I2C access
      These functions let you use I2C interface with currently only one external device addressed at the begin.
      @{
    */

    /*! Start I2C operations.
      Forces I2C pins SDA and SCL
      to alternate function ALT0, which enables those pins for I2C interface.
      You should call  am65x_i2c_end() when all I2C functions are complete to return the pins to
      their default functions
      \return 1 if successful, 0 otherwise (perhaps because you are not running as root)
      \sa   am65x_i2c_end()
    */
    extern int  am65x_i2c_begin(unsigned char bus, unsigned char addr);

    /*! End I2C operations.
      I2C pins P1-03 (SDA) and P1-05 (SCL)
      are returned to their default INPUT behaviour.
    */
    extern void  am65x_i2c_end(void);

    /*! Transfers any number of bytes to the currently selected I2C slave.
      (as previously set by \sa  am65x_i2c_setSlaveAddress)
      \param[in] buf Buffer of bytes to send.
      \param[in] len Number of bytes in the buf buffer, and the number of bytes to send.
      \return reason see \ref  am65xI2CReasonCodes
    */

	extern uint8_t  am65x_i2c_smbus_write(unsigned char * buf, uint32_t len);
    extern uint8_t  am65x_i2c_write(unsigned char * buf, uint32_t len);

    /*! Transfers any number of bytes from the currently selected I2C slave.
      (as previously set by \sa  am65x_i2c_setSlaveAddress)
      \param[in] buf Buffer of bytes to receive.
      \param[in] len Number of bytes in the buf buffer, and the number of bytes to received.
      \return reason see \ref  am65xI2CReasonCodes
    */
	extern uint8_t  am65x_i2c_smbus_read( unsigned char* buf, uint32_t len);
    extern uint8_t  am65x_i2c_read( unsigned char* buf, uint32_t len);
    /*! @}  */
#ifdef __cplusplus
}
#endif

#endif /*  am65x_H */
