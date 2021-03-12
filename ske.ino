#define T_PIN 5
#define H_PIN 5

typedef struct
{
  byte size_; // number of bytes
  byte pin; //pin where we will read the data from
  byte offset;
  byte *data;
  char *type; //type of data as a string
}Module;

byte read_data(byte pin)
{
  byte result = 0;
  for (byte i = 0; i < 8; i++) 
  {
      while (digitalRead(pin) == LOW); // wait 50us
      
      delayMicroseconds(30); //The duration of the high lvl is judged to determine whether the data is '0' or '1'
    
      result |= ((digitalRead(pin) == HIGH << (8 - i)) & (1 << (8 - i))); //Hihg in the former, low in the post
  
      while (digitalRead(pin) == HIGH); //Data '1', waiting for the next bit of reception
  }
  return result;
}

void fill_data(Module *module)
{
  
  digitalWrite(module->pin, LOW); //Pull down the bus to send the start signal
  delay(30); //The delay is greater than 18 ms so that DHT 11 can detect the start signal
  digitalWrite(module->pin, HIGH);
  delayMicroseconds(40); //Wait for DHT11 to respond
  
  pinMode(module->pin, INPUT);
  while(digitalRead(module->pin) == HIGH);
  delayMicroseconds(80); //The DHT11 responds by pulling the bus low for 80us;
  
  if(digitalRead(module->pin) == LOW)
    delayMicroseconds(80); //DHT11 pulled up after the bus 80us to start sending data;
  
  for(int i = module->offset; i < module->size_; module->data[i++] = read_data(module->pin)) //Receiving temperature and humidity data, check bits are not considered;
  
  pinMode(module->pin, OUTPUT);
  digitalWrite(module->pin, HIGH); //After the completion of a release of data bus, waiting for the host to start the next signal
}

void print_data(Module *module)
{
  Serial.print(module->type);  
  for(byte i = 0;i < module->size_;i++)
    Serial.println(module->data[i], DEC);
  Serial.print(" ");
}

void str_cpy(char *l, const char *r)
{
  while(*r)
    *l = *r, l++, r++;
}

void create_module(Module *module, byte size_, int PIN, byte offset, char *str, byte n)
{
  module->size_ = size_;
  module->pin = PIN;
  module->offset = offset;
  module->type = (char*)malloc(sizeof module->type * n + 1);  
  module->data = (byte*)malloc(sizeof(byte) * module->size_);
  str_cpy(module->type, str);
}


Module *temperature;
Module *humidity;

void setup()
{
  Serial.begin(9600);
  
  pinMode(T_PIN, OUTPUT);
  pinMode(H_PIN, OUTPUT);

  create_module(temperature, 2, T_PIN, 2, "Temperature", sizeof("Temperature"));
  create_module(humidity, 2, H_PIN, 0, "Humidity", sizeof("Humidity"));
}

void loop()
{
  fill_data(temperature);
  print_data(temperature);

  fill_data(humidity);
  print_data(temperature);
}
