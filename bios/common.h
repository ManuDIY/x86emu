#ifndef _COMMON_H
#define _COMMON_H

#define uint8_t		unsigned char
#define uint16_t	unsigned short
#define uint32_t	unsigned long
#define bool 		uint8_t
#define true		1
#define false		0

void write_esb(void* vaddr, uint8_t v);
void write_esw(void* vaddr, uint16_t v);
void write_esd(void* vaddr, uint32_t v);
uint8_t in_port(uint16_t port);
void out_port(uint16_t port, uint8_t v);
void cli(void);
void sti(void);

void bsv_test(void);
void bsv_disk(void);
void bsv_irq_disk(void);

#endif
