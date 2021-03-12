#define DEBUG 1

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
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" start read_data");
  #endif
  
  byte result = 0;
  for (byte i = 0; i < 8; i++) 
  {
      while (digitalRead(pin) == LOW); // wait 50us
      
      delayMicroseconds(30); //The duration of the high lvl is judged to determine whether the data is '0' or '1'
    
      result |= ((digitalRead(pin) == HIGH << (8 - i)) & (1 << (8 - i))); //Hihg in the former, low in the post

      #ifdef DEBUG
        Serial.print(millis(), DEC);
        Serial.println(result);
        Serial.println(" start read_data");
      #endif
  
      while (digitalRead(pin) == HIGH){
        break;
        //Data '1', waiting for the next bit of reception
      }
  }
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(result, DEC);
    Serial.println("end read_data");
  #endif
  return result;
}

void fill_data(Module *module)
{
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" start fill_data");
  #endif
  pinMode(module->pin, OUTPUT); 
  digitalWrite(module->pin, LOW); //Pull down the bus to send the start signal
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" mid fill_data");
  #endif
  delay(30); //The delay is greater than 18 ms so that DHT 11 can detect the start signal
  digitalWrite(module->pin, HIGH);
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" mid1 fill_data");
  #endif
  delayMicroseconds(40); //Wait for DHT11 to respond

   #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" mid2 fill_data");
  #endif

  
  pinMode(module->pin, INPUT);

  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.print(module->pin, DEC);
    Serial.println(" mid3 fill_data");
  #endif
  
  
  while(digitalRead(module->pin) == HIGH)
  {
    delayMicroseconds(80);
    #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.print(" module = ");
    Serial.println(digitalRead(module->pin), DEC);
  #endif
    
   break;
  }
  delayMicroseconds(80); //The DHT11 responds by pulling the bus low for 80us;

  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" mid4 fill_data");
  #endif
  
  if(digitalRead(module->pin) == LOW)
    delayMicroseconds(80); //DHT11 pulled up after the bus 80us to start sending data;
  
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" start fill_data");
  #endif
  
  for(int i = module->offset; i < module->size_; i++) //Receiving temperature and humidity data, check bits are not considered;
    module->data[i] = read_data(module->pin);

  
    
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

/*void create_module(Module *module, byte size_, int PIN, byte offset, char *str, byte n)
{
  Module *new_module = (Module*)malloc(sizeof *new_module + n + size_ + 1);
  new_module->size_ = size_;
  new_module->pin = PIN;
  new_module->offset = offset;
  new_module->type = (char*)malloc(sizeof module->type * n + 1);  
  new_module->data = (byte*)malloc(sizeof(byte) * module->size_);
  str_cpy(new_module->type, str);
  module = new_module;
  Serial.println("creating");
}*/

void create_module(Module *module, byte size_, int PIN, byte offset, char *str, byte n)
{
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" start create_module");
  #endif

  module->size_ = size_;
  module->pin = PIN;
  module->offset = offset;
  module->type = (char*)malloc(sizeof module->type * n + 1);  
  module->data = (byte*)malloc(sizeof(byte) * module->size_);
  str_cpy(module->type, str);

  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.print(" ");
    Serial.println(module->type[0]);
    Serial.println(module->type[1]);
    Serial.println(module->type[2]);
    Serial.println(module->type[3]);
    Serial.println(" end create_module");
  #endif
}

Module temperature, humidity;

void setup()
{
  Serial.begin(9600);
  
  pinMode(T_PIN, OUTPUT);
  pinMode(H_PIN, OUTPUT);

  create_module(&temperature, 2, T_PIN, 2, "Temperature", sizeof("Temperature"));
  create_module(&humidity, 2, H_PIN, 0, "Humidity", sizeof("Humidity"));
}

void loop()
{
  #ifdef DEBUG
    Serial.print(millis(), DEC);
    Serial.println(" Test");
  #endif

  
  fill_data(&temperature);
  print_data(&temperature);

  fill_data(&humidity);
  print_data(&humidity);
  delay(1000);
}
