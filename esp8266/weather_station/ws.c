
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

#include "dht22.h"

static const int dht_pin = 14;
static volatile os_timer_t some_timer;

void business_timerfunc(void *arg) {
  float temp = readTemperature(false);
  os_printf("temperature now: %f\n", temp);
}

void ICACHE_FLASH_ATTR user_init() {
  // init gpio subsytem
  gpio_init();

  DHT_init(14, DHT22, 1);

  os_timer_setfn(&some_timer, (os_timer_func_t *)business_timerfunc, NULL);
  os_timer_arm(&some_timer, 2000, 1);
}
