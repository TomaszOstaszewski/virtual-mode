#include "pic-8259a.h"
#include "common.h"

#define PIC_MASTER_A0_LOW  (0x0020)
#define PIC_MASTER_A0_HIGH (0x0021)
#define PIC_SLAVE_A0_LOW  (0x00A0)
#define PIC_SLAVE_A0_HIGH (0x00A1)

#define PIC_ICW2_MASK (0xf8)

typedef enum pic_icw1 {
		pic_icw1_icw4 = 0x11,
		pic_icw1_sngl = 0x12,
		pic_icw1_ltim = 0x14
} pic_icw1_t;

#define PIC_MASTER_ICW3 (0x04)
#define PIC_SLAVE_ICW3 (0x02)

typedef enum pic_icw4 {
		pic_icw4_8086_mode          = 0x01,
		pic_icw4_aeoi               = 0x02,
		pic_icw4_non_buffered       = 0x00,
		pic_icw4_master_buffered    = 0x08,
		pic_icw4_slave_buffered     = 0x0c,
		pic_icw4_snfm               = 0x10,
} pic_icw4_t;

#define PIC_OCW1_ALL_INTS_ENABLED (0x00)
#define PIC_OCW2_EOI (0x20)

uint8_t g_master_offset;
uint8_t g_slave_offset;

/*! 
 * @brief Intel Programmable Interrupt Controller (PIC for short) initialization routine.
 * @details IBM PC AT has 2 PICs, the master and the slave. We need to initialize
 * both of them. Initialization starts with sending the "Initialization Control Word 1". 
 * Both PICs are connected to the address bus, the data bus and the IO bus. Each one of them
 * will recognize every byte send to them as ICW1, if the following conditions are met:
 * \li the A0 (address bus bit 0) line is 0;
 * \li the D4 (4th bit staring counting from 1) is 1;
 * The ICW1 format is as follows (for ubiquitous 8086 mode):
 * <tt>bit  | 7 | 6 | 5 | 4 | 3    | 2 | 1    | 0   |</tt>
 * <tt>value    | 0 | 0 | 0 | 1 | LTIM | 0 | SNGL | IC4 |</tt>
 * where:
 * \li \c LTIM stands for Level Triggered Interrupt Mode, hence is 1 for level triggered mode, 0 for edge triggered; 
 * \li \c SNGL stands for Single or cascade, hence 1 for single PIC mode, 0 for cascade PIC mode.
 * \li \c IC4 stands for Initialization Control Word 4 needed , hence 1 if we are going to send IC4, 0 otherwise.
 * For standard PC/AT PIC we are going to have:
 * \l1  edge triggered interrupts;
 * \l1  cascade mode;
 * \l1  we do send IC4;
 * Given all that, the very first thing we send to both PICs is byte 00010001b = 0x11. Remembering that A0 line must
 * be held low, we output 0x11 to ports (addresses) 0x20 for master PIC and 0xA0 for the slave PIC.
 * When a PIC recognizes ICW1, it expects at least ICW2 and ICW3 to follow. If indicated, as we usually do, that ICW4 follows, then it must be send as well.
 * After initialization is complete, one may read or write Operation Control Words (OCWs for short) in any order, until she or he sends ICW1 again. 
 * Next initialization word to be send is ICW2, whose format is as follows (for 8086 ubiquitous mode):
 * <tt>bit  | 7  | 6  | 5  | 4  | 3  | 2 | 1 | 0 |</tt>
 * <tt>value    | T7 | T6 | T5 | T4 | T3 | 0 | 0 | 0 |</tt>
 * where \c T7 .. \c T3 are the bits being placed on the data bus during interrupt acknowledge cycle. The three least significant bits will be equal to the interrupt being issued.
 * Hence, if for instance, we send ICW2 as 0x20 to PIC1, then it will send 0x20 whenever interrput 1 is signaled, and 0x21 when interrupt 2 is signaled, and so on.
 * In protected mode, we usually configure PIC so that master ICW2 points to 0x20 and slave's ICW2 pointing right after it at 0x28. As the D4 bit may be sometimes set to 1, we
 * must send this ICW2 with A0 line held high. Therefore, we do send ICW2 to port 0x21 for master PIC and to port 0xA1 to slave PIC.
 * \p
 * Then goes Initialization Control Word 3. This is actually used in in PC AT. This one is here to tell each PIC about each other. Its meaning its different for the master and the slave. 
 * For the master it is a bit mask that tells which input is hooked up to slave controller. For slave it's the same, but it's encoding differs - its three least significant bits are the numerical value of that
 * input. For IBM PC/AT machine, the IBM engineers decided for us. The hooked up slave PIC so that it is connected to interrupt request line 2 of the slave PIC (interrupt request lines are 0..7).
 * It seems like we need to send 00000100b = 0x04 to the master and 0x02 to the slave. Again, to avoid clash with ICW1, A0 line must be held high. Otherwise, a PIC logic may decide that this is ICW1 and we would need 
 * to start initialization all over again.
 * \p 
 * Finally goes the Initialization Control Word 4, ICW4 for short. This is pretty important, because it actually tells a PIC, among other things, in which mode it will work from now on. 
 * If we don't send it, then it will assume that it's mode is MCS 8080/8085. In short, without ICW4 your PC, whether it is XT or AT, "will be in a world of shit" and your system will die without permission. 
 * Format of the ICW4 is as follows:
 * <tt>bit  | 7 | 6 | 5 | 4    | 3  | 2 | 1    | 0    |</tt>
 * <tt>value    | 0 | 0 | 0 | SNFM | BUFMS  | AEOI | miPM |</tt>
 * which stand for:
 * \l1 \c SNFM - Special Fully Nested Mode, hence 1 for Special Fully Nested Mode and 0 for not Special Fully Nested Mode. 
 * \l1 \c BUFMS - a pair of bits that indicate whether it is Non buffered mode (b0x = b0whatever = 00 or 01), buffered mode master (10b) or buffered mode slave (11b)
 * \l1 \c AEOI - automatic End Of Interrupt, hence 1 for AEOI and 0 for standard EOI.
 * \l1 \c miPM - stands for microprocessor Programming Mode, hence 1 for 8086 and 0 for MCS 8080/8085.
 * Usually, we only indicate 8086 mode in our ICW4 being send on PC. That means that we need to send 00000001b = 0x01 to both PICs.
 * Again, to avoid clash with ICW1 again, we need to send this ICW4 with A0 line held high.
 * To sum up:
 * - we send the ICW1 with A0 held low to both PICs.
 * - we send the ICW2s with A0 held high to both PICs.
 * - we send the ICW3s with A0 held high to both PICs.
 * - we send the ICW4s with A0 held high to both PICs.
 * after all those chores are completed, we are done initializing our PIC. From now on, until ICW1 is submitted again, all reads and writes 
 * will be Operation Control Words, OCWs for short.
 * @param master_offset - interrupt vector offset for the master controller.
 * @param slave_offset- interrupt vector offset for the slave controller.
 */
void pic_init(uint8_t master_offset, uint8_t slave_offset)
{    
		g_master_offset   &= PIC_ICW2_MASK;
		g_slave_offset    &= PIC_ICW2_MASK;
		/* Send ICW1 to the master */
		outb(PIC_MASTER_A0_LOW, pic_icw1_icw4); 
		/* Send ICW1 to the slave */
		outb(PIC_SLAVE_A0_LOW, pic_icw1_icw4); 
		/* Send ICW2 to the master */
		outb(PIC_MASTER_A0_HIGH, g_master_offset); 
		/* Send ICW2 to the slave */
		outb(PIC_SLAVE_A0_HIGH, g_slave_offset); 
		/* Send ICW3 to the master */
		outb(PIC_MASTER_A0_HIGH, PIC_MASTER_ICW3); 
		/* Send ICW3 to the slave */
		outb(PIC_SLAVE_A0_HIGH, PIC_SLAVE_ICW3); 
		/* Finally, send both of them our ICW4 */   
		/* ICW4 for master */
		outb(PIC_MASTER_A0_HIGH, pic_icw4_8086_mode); 
		/* ICW4 for slave */
		outb(PIC_SLAVE_A0_HIGH, pic_icw4_8086_mode); 
		/* Enable all external interrupts for both PICs */
		outb(PIC_MASTER_A0_HIGH, PIC_OCW1_ALL_INTS_ENABLED); 
		outb(PIC_SLAVE_A0_HIGH, PIC_OCW1_ALL_INTS_ENABLED); 
}

/*!
 * @brief Returns PIC interrupt mask.
 * @details PIC interrupt mask is a special Operation Control Word 1, a byte whose can be read or written to PIC. PIC recognizes that a request is for OCW1 if the A0 line is held high.
 * Format of the OCW1 is pretty simple - it is a 8 bit mask, whose bits set to 1 at various positions do indicate that an input does not accept interrupts.
 * @param
 * @retval
 */
uint8_t pic_get_mask(pic_8259a_type pic_type)
{
		uint8_t mask = 0x00;
		switch (pic_type)
		{
				case PIC_MASTER:
						mask = inb(PIC_MASTER_A0_HIGH); 
						break;
				case PIC_SLAVE:
						mask = inb(PIC_SLAVE_A0_HIGH); 
						break;
		}
		return mask;
}

/*!
 * @brief Returns PIC interrupt mask.
 * @details PIC interrupt mask is a special Operation Control Word 1, a byte whose can be read or written to PIC. PIC recognizes that a request is for OCW1 if the A0 line is held high.
 * Format of the OCW1 is pretty simple - it is a 8 bit mask, whose bits set to 1 at various positions do indicate that an input does not accept interrupts.
 * @param
 * @retval
 */
void pic_set_mask(pic_8259a_type pic_type, uint8_t mask)
{
		switch (pic_type)
		{
				case PIC_MASTER:
						outb(PIC_MASTER_A0_HIGH, mask); 
						break;
				case PIC_SLAVE:
						outb(PIC_SLAVE_A0_HIGH, mask); 
						break;
		}
}

/*!
 * @brief Sends the End Of Interrupt to the master PIC or both to the master and the slave PIC.
 * @details The EOI is just a single bit in Operation Control Word 2. Other bits are unused in 8086 system. Until EOI is send, 
 * all the interrupts of the same or lower priority will be inhibited.
 * @param irq_num - interrupt number, used to determine whether to send EOI to just master or to both PICs.
 */
void pic_send_eoi(int irq_num)
{
		if (irq_num >= g_slave_offset)
		{
				outb(PIC_SLAVE_A0_LOW, PIC_OCW2_EOI); 
		}
		outb(PIC_MASTER_A0_LOW, PIC_OCW2_EOI); 
}

#define PIC_READ_IR (0x02)

#define PIC_READ_IS (0x03)

/*! \todo Prevent from simultaneous access */
uint8_t pic_read_IR(pic_8259a_type pic_type)
{
		uint8_t ir;
		ir = 0;
		switch (pic_type)
		{
				/* \todo Prevent from simultaneous access */
				case PIC_SLAVE:
						outb(PIC_SLAVE_A0_LOW, PIC_READ_IR); 
						ir = inb(PIC_SLAVE_A0_LOW);
						break; 
						/* \todo Prevent from simultaneous access */
				case PIC_MASTER:
						outb(PIC_MASTER_A0_LOW, PIC_READ_IR); 
						ir = inb(PIC_MASTER_A0_LOW);
						break; 
		}
		return ir;
}

/*! \todo Prevent from simultaneous access */
uint8_t pic_read_ISR(pic_8259a_type pic_type)
{
		uint8_t is;
		is = 0;
		switch (pic_type)
		{
				case PIC_SLAVE:
						outb(PIC_SLAVE_A0_LOW, PIC_READ_IS); 
						is = inb(PIC_MASTER_A0_LOW);
						break;
				case PIC_MASTER:
						outb(PIC_MASTER_A0_LOW, PIC_READ_IS); 
						is = inb(PIC_MASTER_A0_LOW);
						break;
		}
		return is;
}

