#if !defined PIC_8259A_B73BB3C1_94C9_436C_B8E6_D2A31A2EA61C
#define PIC_8259A_B73BB3C1_94C9_436C_B8E6_D2A31A2EA61C

#include <stdint.h>

typedef enum pic_8259a_type_t {
	PIC_MASTER = 0x00,
	PIC_SLAVE =  0x02,
} pic_8259a_type;

void pic_init(uint8_t master_offset, uint8_t slave_offset);

uint8_t pic_get_mask(pic_8259a_type pic_type);

void pic_set_mask(pic_8259a_type pic_type, uint8_t mask);

void pic_send_eoi(int irq_num);

uint8_t pic_read_IR(pic_8259a_type pic_type);

uint8_t pic_read_IS(pic_8259a_type pic_type);

#endif /* defined PIC_8259A_B73BB3C1_94C9_436C_B8E6_D2A31A2EA61C */
