/**
 *
 */
// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
//           Made for JamesM's tutorials <www.jamesmolloy.co.uk>

#include "monitor.h"
#include "intrisics.h"

struct multiboot;

void parse_cr0(uint32_t cr0) {
  int paging = cr0 >> 31;
  int cd = (cr0 >> 30) & 0x1;
  int nw = (cr0 >> 29) & 0x1;
  int am = (cr0 >> 18) & 0x1;
  int wp = (cr0 >> 16) & 0x1;
  int ne = (cr0 >> 5) & 0x1;
  int et = (cr0 >> 4) & 0x1;
  int pe = (cr0 >> 0) & 0x1;
  monitor_write("paging:=");
  monitor_write_hex(paging);
  monitor_write("\ncache disabled:=");
  monitor_write_hex(cd);
  monitor_write("\nnot write-through:=");
  monitor_write_hex(nw);
  monitor_write("\nalignment-mask:=");
  monitor_write_hex(am);
  monitor_write("\nwrite-protect:=");
  monitor_write_hex(wp);
  monitor_write("\nnumeric-error:=");
  monitor_write_hex(ne);
  monitor_write("\nextension-type:=");
  monitor_write_hex(et);
  monitor_write("\nprotected-mode:=");
  monitor_write_hex(pe);

  
}

int kernel_main(void) {
    monitor_write("Hello, world!\n");
    monitor_write_hex(read_cr0());
    monitor_put('\n');
    monitor_write("Interrupts : ");
    if (are_interrupts_enabled()) {
      monitor_write("enabled\n");
    } else {
      monitor_write("disabled\n");
    }
    parse_cr0(read_cr0());
    return 0;
}
