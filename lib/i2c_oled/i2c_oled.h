
void i2c_OLED_send_cmd(uint8_t command);
void i2c_OLED_send_byte(uint8_t val) ;
void  i2c_OLED_init(void);
void i2c_OLED_send_char(unsigned char ascii);;
void i2c_OLED_send_string(const char *string);
//void i2c_OLED_send_logo();
// void i2c_OLED_Put_Logo();
void i2c_OLED_set_XY(byte col, byte row);
void i2c_OLED_set_line(byte row);
void i2c_clear_OLED(void);
void i2c_writeReg(uint8_t add, uint8_t reg, uint8_t val);
void waitTransmissionI2C(void);
void i2c_stop(void);
void i2c_write(uint8_t data);
void i2c_rep_start(uint8_t address);
