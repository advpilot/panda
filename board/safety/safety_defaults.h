void mdps_spoof_speed(CAN_FIFOMailBox_TypeDef *to_fwd){
  bool mph_speed = GET_BYTE(to_fwd, 2) & 0x2;
  int enable_speed = mph_speed ? 38 : 60;
  int speed = (GET_BYTE(to_fwd, 1) | (GET_BYTE(to_fwd, 2) & 0x1) << 8) * 0.5;
  if (speed < enable_speed) {
    to_fwd->RDLR &= 0xFFFE00FF;
    to_fwd->RDLR |= (enable_speed * 2) << 8;
  }
};

void default_rx_hook(CAN_FIFOMailBox_TypeDef *to_push) {
  UNUSED(to_push);
}

// *** no output safety mode ***

static void nooutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = 0;
}

static int nooutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return false;
}

static int nooutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return false;
}

static int default_fwd_hook(int bus_num, CAN_FIFOMailBox_TypeDef *to_fwd) {
  UNUSED(to_fwd);
  int bus_fwd = -1;
  // CAN1 to CAN3
  if (bus_num == 0) {
    bus_fwd = 2;
    // modify speed
    if (addr == 1265) {
      mdps_spoof_speed(to_fwd);
    }
  }
  // CAN3 to CAN1
  if (bus_num == 2) {
    bus_fwd = 0;
  }
  return bus_fwd;
}

const safety_hooks nooutput_hooks = {
  .init = nooutput_init,
  .rx = default_rx_hook,
  .tx = nooutput_tx_hook,
  .tx_lin = nooutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};

// *** all output safety mode ***

static void alloutput_init(int16_t param) {
  UNUSED(param);
  controls_allowed = 1;
}

static int alloutput_tx_hook(CAN_FIFOMailBox_TypeDef *to_send) {
  UNUSED(to_send);
  return true;
}

static int alloutput_tx_lin_hook(int lin_num, uint8_t *data, int len) {
  UNUSED(lin_num);
  UNUSED(data);
  UNUSED(len);
  return true;
}

const safety_hooks alloutput_hooks = {
  .init = alloutput_init,
  .rx = default_rx_hook,
  .tx = alloutput_tx_hook,
  .tx_lin = alloutput_tx_lin_hook,
  .fwd = default_fwd_hook,
};
