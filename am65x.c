
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#define BCK2835_LIBRARY_BUILD
#include "am65x.h"



unsigned int am65x_version(void) 
{
    return AM65X_VERSION;
}

/* Read with memory barriers from peripheral
 *
 */
uint32_t am65x_peri_read(volatile uint32_t* paddr)
{
    uint32_t ret;
    if (debug)
    {
		printf(" am65x_peri_read  paddr %p\n", (void *) paddr);
		return 0;
    }
    else
    {
       __sync_synchronize();
       ret = *paddr;
       __sync_synchronize();
       return ret;
    }
}


uint32_t  am65x_peri_read(volatile uint32_t* paddr)
{
    if (debug)
    {
	printf(" am65x_peri_read_nb  paddr %p\n", paddr);
	return 0;
    }
    else
    {
	return *paddr;
    }
}


void  am65x_peri_write(volatile uint32_t* paddr, uint32_t value)
{
    if (debug)
    {
	printf(" am65x_peri_write_nb paddr %p, value %08X\n",
                paddr, value);
    }
    else
    {
	*paddr = value;
    }
}

/* Set/clear only the bits in value covered by the mask
 * This is not atomic - can be interrupted.
 */
void  am65x_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask)
{
    uint32_t v =  am65x_peri_read(paddr);
    v = (v & ~mask) | (value & mask);
     am65x_peri_write(paddr, v);
}


int  am65x_i2c_begin(void)
{
    uint16_t cdiv;

    if (    am65x_bsc0 == MAP_FAILED
	||  am65x_bsc1 == MAP_FAILED)
      return 0; /*  am65x_init() failed, or not root */

    volatile uint32_t* paddr =  am65x_bsc1 + am65x_I2C_DIV/4;
    /* Set the I2C/BSC1 pins to the Alt 0 function to enable I2C access on them */
     //am65x_gpio_fsel(RPI_V2_GPIO_P1_03,  am65x_GPIO_FSEL_ALT0); /* SDA */
     //am65x_gpio_fsel(RPI_V2_GPIO_P1_05,  am65x_GPIO_FSEL_ALT0); /* SCL */ 

    /* Read the clock divider register */
    cdiv =  am65x_peri_read(paddr);
    /* Calculate time for transmitting one byte
    // 1000000 = micros seconds in a second
    // 9 = Clocks per byte : 8 bits + ACK
    */
    i2c_byte_wait_us = ((float)cdiv /  am65x_CORE_CLK_HZ) * 1000000 * 9;

    return 1;
}

void  am65x_i2c_end(void)
{
    /* Set all the I2C/BSC1 pins back to input */
     //am65x_gpio_fsel(RPI_V2_GPIO_P1_03,  am65x_GPIO_FSEL_INPT); /* SDA */
     //am65x_gpio_fsel(RPI_V2_GPIO_P1_05,  am65x_GPIO_FSEL_INPT); /* SCL */
}

void  am65x_i2c_setSlaveAddress(uint8_t addr)
{
    /* Set I2C Device Address */
    volatile uint32_t* paddr =  am65x_I2C1 +  am65x_I2C_A/4;
     am65x_peri_write(paddr, addr);
}

/* defaults to 0x5dc, should result in a 166.666 kHz I2C clock frequency.
// The divisor must be a power of 2. Odd numbers
// rounded down.
*/
void  am65x_i2c_setClockDivider(uint16_t divider)
{
    volatile uint32_t* paddr =  am65x_I2C1 +  am65x_I2C_DIV/4;    
     am65x_peri_write(paddr, divider);
    /* Calculate time for transmitting one byte
    // 1000000 = micros seconds in a second
    // 9 = Clocks per byte : 8 bits + ACK
    */
    i2c_byte_wait_us = ((float)divider /  am65x_CORE_CLK_HZ) * 1000000 * 9;
}

/* set I2C clock divider by means of a baudrate number */
void  am65x_i2c_set_baudrate(uint32_t baudrate)
{
	uint32_t divider;
	/* use 0xFFFE mask to limit a max value and round down any odd number */
	divider = ( am65x_CORE_CLK_HZ / baudrate) & 0xFFFE;
	am65x_i2c_setClockDivider( (uint16_t)divider );
}

/* Writes an number of bytes to I2C */
uint8_t  am65x_i2c_write(const char * buf, uint32_t len)
{
    volatile uint32_t* dlen    =  am65x_bsc1 +  am65x_I2C_DLEN/4;
    volatile uint32_t* fifo    =  am65x_bsc1 +  am65x_I2C_FIFO/4;
    volatile uint32_t* status  =  am65x_bsc1 +  am65x_I2C_S/4;
    volatile uint32_t* control =  am65x_bsc1 +  am65x_I2C_C/4;  

    uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason =  am65x_I2C_REASON_OK;

    /* Clear FIFO */
     am65x_peri_set_bits(control,  am65x_I2C_C_CLEAR_1 ,  am65x_I2C_C_CLEAR_1 );
    /* Clear Status */
     am65x_peri_write(status,  am65x_I2C_S_CLKT |  am65x_I2C_S_ERR |  am65x_I2C_S_DONE);
    /* Set Data Length */
     am65x_peri_write(dlen, len);
    /* pre populate FIFO with max buffer */
    while( remaining && ( i <  am65x_I2C_FIFO_SIZE ) )
    {
         am65x_peri_write_nb(fifo, buf[i]);
        i++;
        remaining--;
    }
    
    /* Enable device and start transfer */
     am65x_peri_write(control,  am65x_I2C_C_I2CEN |  am65x_I2C_C_ST);
    
    /* Transfer is over when  am65x_BSC_S_DONE */
    while(!(am65x_peri_read(status) &  am65x_I2C_S_DONE ))
    {
        while (remaining && ( am65x_peri_read(status) &  am65x_I2C_S_TXD ))
    	{
			/* Write to FIFO */
			am65x_peri_write(fifo, buf[i]);
			i++;
			remaining--;
    	}
    }

    /* Received a NACK */
    if ( am65x_peri_read(status) &  am65x_I2C_S_ERR)
    {
	reason =  am65x_I2C_REASON_ERROR_NACK;
    }

    /* Received Clock Stretch Timeout */
    else if ( am65x_peri_read(status) &  am65x_I2C_S_CLKT)
    {
	reason =  am65x_I2C_REASON_ERROR_CLKT;
    }

    /* Not all data is sent */
    else if (remaining)
    {
	reason =  am65x_I2C_REASON_ERROR_DATA;
    }

     am65x_peri_set_bits(control,  am65x_I2C_S_DONE ,  am65x_I2C_S_DONE);

    return reason;
}

/* Read an number of bytes from I2C */
uint8_t  am65x_i2c_read(char* buf, uint32_t len)
{

    volatile uint32_t* dlen    =  am65x_I2C1 +  am65x_I2C_DLEN/4;
    volatile uint32_t* fifo    =  am65x_I2C1 +  am65x_I2C_FIFO/4;
    volatile uint32_t* status  =  am65x_I2C1 +  am65x_I2C_S/4;
    volatile uint32_t* control =  am65x_I2C1 +  am65x_I2C_C/4;

    uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason =  am65x_I2C_REASON_OK;

    /* Clear FIFO */
     am65x_peri_set_bits(control,  am65x_I2C_C_CLEAR_1 ,  am65x_I2C_C_CLEAR_1 );
    /* Clear Status */
     am65x_peri_write(status,  am65x_I2C_S_CLKT |  am65x_I2C_S_ERR |  am65x_I2C_S_DONE);
    /* Set Data Length */
     am65x_peri_write(dlen, len);
    /* Start read */
     am65x_peri_write(control,  am65x_I2C_C_I2CEN |  am65x_I2C_C_ST |  am65x_I2C_C_READ);
    
    /* wait for transfer to complete */
    while (!( am65x_peri_read(status) &  am65x_I2C_S_DONE))
    {
        /* we must empty the FIFO as it is populated and not use any delay */
        while (remaining &&  am65x_peri_read(status) &  am65x_I2C_S_RXD)
    	{
			/* Read from FIFO, no barrier */
			buf[i] =  am65x_peri_read(fifo);
			i++;
			remaining--;
    	}
    }
    
    /* transfer has finished - grab any remaining stuff in FIFO */
    while (remaining && ( am65x_peri_read(status) &  am65x_I2C_S_RXD))
    {
        /* Read from FIFO, no barrier */
        buf[i] =  am65x_peri_read(fifo);
        i++;
        remaining--;
    }
    
    /* Received a NACK */
    if (am65x_peri_read(status) &  am65x_I2C_S_ERR)
    {
		reason =  am65x_I2C_REASON_ERROR_NACK;
    }

    /* Received Clock Stretch Timeout */
    else if (am65x_peri_read(status) &  am65x_I2C_S_CLKT)
    {
		reason =  am65x_I2C_REASON_ERROR_CLKT;
    }

    /* Not all data is received */
    else if (remaining)
    {
		reason =  am65x_I2C_REASON_ERROR_DATA;
    }

     am65x_peri_set_bits(status,  am65x_BSC_S_DONE ,  am65x_BSC_S_DONE);

    return reason;
}

/* Read the System Timer Counter (64-bits) */
uint64_t  am65x_st_read(void)
{
    volatile uint32_t* paddr;
    uint32_t hi, lo;
    uint64_t st;

    if (am65x_st==MAP_FAILED)
		return 0;

    paddr =  am65x_st +  am65x_ST_CHI/4;
    hi =  am65x_peri_read(paddr);

    paddr =  am65x_st +  am65x_ST_CLO/4;
    lo =  am65x_peri_read(paddr);
    
    paddr =  am65x_st +  am65x_ST_CHI/4;
    st =  am65x_peri_read(paddr);
    
    /* Test for overflow */
    if (st == hi)
    {
        st <<= 32;
        st += lo;
    }
    else
    {
        st <<= 32;
        paddr =  am65x_st +  am65x_ST_CLO/4;
        st +=  am65x_peri_read(paddr);
    }
    return st;
}

/* Delays for the specified number of microseconds with offset */
void  am65x_st_delay(uint64_t offset_micros, uint64_t micros)
{
    uint64_t compare = offset_micros + micros;

    while(am65x_st_read() < compare)
	;
}

/* Allocate page-aligned memory. */
void *malloc_aligned(size_t size)
{
    void *mem;
    errno = posix_memalign(&mem,  am65x_PAGE_SIZE, size);
    return (errno ? NULL : mem);
}

/* Map 'size' bytes starting at 'off' in file 'fd' to memory.
// Return mapped address on success, MAP_FAILED otherwise.
// On error print message.
*/
static void *mapmem(const char *msg, size_t size, int fd, off_t off)
{
    void *map = mmap(NULL, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, off);
    if (map == MAP_FAILED)
		fprintf(stderr, " am65x_init: %s mmap failed: %s\n", msg, strerror(errno));
    return map;
}

static void unmapmem(void **pmem, size_t size)
{
    if (*pmem == MAP_FAILED) return;
		munmap(*pmem, size);
    *pmem = MAP_FAILED;
}

/* Initialise this library. */
int  am65x_init(void)
{
    int  memfd;
    int  ok;
    FILE *fp;

    /* Now get ready to map the peripherals block 
     * If we are not root, try for the new /dev/gpiomem interface and accept
     * the fact that we can only access GPIO
     * else try for the /dev/mem interface and get access to everything
     */
    memfd = -1;
    ok = 0;
    if (geteuid() == 0
#ifdef  am65x_HAVE_LIBCAP
	||  am65x_has_capability(CAP_SYS_RAWIO)
#endif
	)
    {
      /* Open the master /dev/mem device */
      if ((memfd = open("/dev/mem", O_RDWR | O_SYNC) ) < 0) 
	{
	  fprintf(stderr, " am65x_init: Unable to open /dev/mem: %s\n",
		  strerror(errno)) ;
	  goto exit;
	}
      
      /* Base of the peripherals block is mapped to VM */
      am65x_peripherals = mapmem("gpio",  am65x_peripherals_size, memfd,  am65x_peripherals_base);
      if ( am65x_peripherals == MAP_FAILED) goto exit;
      
      /* Now compute the base addresses of various peripherals, 
      // which are at fixed offsets within the mapped peripherals block
      // Caution:  am65x_peripherals is uint32_t*, so divide offsets by 4
      */
       am65x_bsc0 =  am65x_peripherals +  am65x_BSC0_BASE/4; /* I2C */
       am65x_bsc1 =  am65x_peripherals +  am65x_BSC1_BASE/4; /* I2C */

      ok = 1;
    }
    else
    {
      /* Not root, try /dev/gpiomem */
      /* Open the master /dev/mem device */
      if ((memfd = open("/dev/gpiomem", O_RDWR | O_SYNC) ) < 0) 
	{
	  fprintf(stderr, " am65x_init: Unable to open /dev/gpiomem: %s\n",
		  strerror(errno)) ;
	  goto exit;
	}
      
      /* Base of the peripherals block is mapped to VM */
       am65x_peripherals_base = 0;
       am65x_peripherals = mapmem("gpio",  am65x_peripherals_size, memfd,  am65x_peripherals_base);
      if ( am65x_peripherals == MAP_FAILED) goto exit;
       am65x_gpio =  am65x_peripherals;
      ok = 1;
    }

exit:
    if (memfd >= 0)
        close(memfd);

    if (!ok)
	 am65x_close();

    return ok;
}

/* Close this library and deallocate everything */
int  am65x_close(void)
{
    return 1; /* Success */
} 



