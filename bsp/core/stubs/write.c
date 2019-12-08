/* See LICENSE of license details. */

/* See LICENSE of license details. */

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "stm32f10x.h"
#include <unistd.h>
#include <sys/types.h>
#include "sys.h"
#include "usart.h"

#include "stub.h"


typedef unsigned int size_t;

// extern int _put_char(int ch) __attribute__((weak));

int _put_char(int ch)
{      
	while((USART1->SR&0X40)==0);// 
	USART1->DR = (u8) ch;      	//дDR,����1����������
	return ch;
}

ssize_t _write(int fd, const void* ptr, size_t len) {
	const uint8_t * current = (const uint8_t *) ptr;

	if (isatty(fd)) 
	{
		for (size_t jj = 0; jj < len; jj++) {
			_put_char(current[jj]);

			if (current[jj] == '\n') {
				_put_char('\r');
			}
		}
		return len;
	}

	return _stub(EBADF);
}
