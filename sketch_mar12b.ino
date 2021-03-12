struct Module
{
  byte size_;
  byte pin;
  byte *data;
  void (*fill_data)(struct Module *module);
  void (*print_data)(const struct Module *module);
};

void print_humidity_temperature(const struct Module *module)
{
  Serial.print("Humdity = ");
  Serial.print(module->data[0], DEC); 
  Serial.print('.');
  Serial.print(module->data[1], DEC); 
  Serial.println('%');
  Serial.print("Temperature = ");
  Serial.print(module->data[2], DEC);
  Serial.print('.');
  Serial.print(module->data[3], DEC);
  Serial.println('C');
  byte checksum = module->data[0] + module->data[1] + module->data[2] + module->data[3];
  if (module->data[4] != checksum) 
    Serial.println("-- Checksum Error!");
  else
    Serial.println("-- OK");
}
void fill_flame_data_module(struct Module *module)
{
  module->data[0] = analogRead(module->pin); 
}
void print_flame_data_module(const struct Module *module)
{
  if(module->data[0] >= 33) // if flame isn't detected
    Serial.println("NO"); // turn OFF Arduino's LED
  else
    Serial.println("YES"); // turn ON Arduino's LED
}

byte read_data_humidity_temperature(byte pin)
{
  byte i = 0;
  byte result = 0;
  for (i = 0; i < 8; i++) {
      while (digitalRead(pin) == LOW);
      delayMicroseconds(30); 
      if (digitalRead(pin) == HIGH)
        result |= (1 << (8 - i)); 
    while (digitalRead(pin) == HIGH);
    }
  return result;
}

void fill_data_humidity_temperature(struct Module *module)
{
  digitalWrite(module->pin, LOW);
  delay(30); 
  digitalWrite(module->pin, HIGH);
  delayMicroseconds(40); 
  pinMode(module->pin, INPUT);
  while(digitalRead(module->pin) == HIGH);
  delayMicroseconds(80); 
  if(digitalRead(module->pin) == LOW)
    delayMicroseconds(80); 
  for(int i = 0; i < module->size_; i++) 
    module->data[i] = read_data_humidity_temperature(module->pin);
  pinMode(module->pin, OUTPUT);
  digitalWrite(module->pin, HIGH);
}

void create_module(struct Module *module, byte size_, byte pin, void (*fill_)(struct Module *module), void (*print_)(const struct Module *module))
{
  module->size_ = size_;
  module->pin = pin;
  module->data = (byte*)malloc(sizeof(byte) * module->size_);
  module->fill_data = fill_;
  module->print_data = print_;
  pinMode(module->pin, OUTPUT);
}

struct Module humidity_temperature_module, flame_module;

void setup()
{
  Serial.begin(9600);
  create_module(&humidity_temperature_module, 4, 5, fill_data_humidity_temperature, print_humidity_temperature);
  
  create_module(&flame_module, 1, A0, fill_flame_data_module, print_flame_data_module);
}

void loop()
{
  //humidity_temperature_module.fill_data(&humidity_temperature_module);
  //humidity_temperature_module.print_data(&humidity_temperature_module);
  flame_module.fill_data(&flame_module);
  flame_module.print_data(&flame_module);
  delay(1000);
}
