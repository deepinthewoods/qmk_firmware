#pragma once

#include_next <mcuconf.h>

#undef RP2040_MCUCONF
#define RP2040_MCUCONF

#undef RP_XOSCCLK
#define RP_XOSCCLK 12000000



#undef RP_IRQ_USB0_PRIORITY
#define RP_IRQ_USB0_PRIORITY 3

#undef RP2040_DEVICE
#define RP2040_DEVICE RP2040