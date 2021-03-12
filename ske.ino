#include<malloc.h>

#define T_PIN 1

typedef struct
{
  byte size; // number of bytes
  byte pin; //pin where we will read the data from
  byte data[]; //value_of_data
  char type[]; //type of data as a string
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

void fill_data(Module module, byte offset)
{
  
  digitalWrite(DHpin, LOW); //Pull down the bus to send the start signal
  delay(30); //The delay is greater than 18 ms so that DHT 11 can detect the start signal
  digitalWrite(DHpin, HIGH);
  delayMicroseconds(40); //Wait for DHT11 to respond
  
  pinMode(DHpin, INPUT);
  while(digitalRead(DHpin) == HIGH);
  delayMicroseconds(80); //The DHT11 responds by pulling the bus low for 80us;
  
  if(digitalRead(DHpin) == LOW)
    delayMicroseconds(80); //DHT11 pulled up after the bus 80us to start sending data;
  
  for(int i = offset; i < module.size; module.data[i++] = read_data(module.pin)) //Receiving temperature and humidity data, check bits are not considered;
  
  pinMode(DHpin, OUTPUT);
  digitalWrite(DHpin, HIGH); //After the completion of a release of data bus, waiting for the host to start the next signal
}

void print_data(Module module)
{
  Serial.print(module.type);  
  for(byte i = 0;i < module.size;i++)
    Serial.println(module.data[i]);
}

byte *get_memory(byte n)
{
  byte *res = malloc(sizeof *res * n);
  return res;
}

void str_cpy(char *l, const char *r)
{
  while(*r)
    *l = *r, l++, r++;
}

byte *get_type(char *str, byte n)
{
  char *type = malloc(sizeof a * n);
  str_cpy(type, str);
  return type;
}

//-------------------------------------------------------------------------------------------------------------------------


void setup()
{
  Serial.begin(9600);
  pinMode(DHpin, OUTPUT);
  Module temperature = {2, T_PIN, get_memory(2), get_type("Temperature", sizeof("Temperature"))};
  Module humidity = {};
}


void loop()
{
  /*get_data();
  
  Serial.print("Humdity = ");
  Serial.print(dat[0], DEC); //Displays the integer bits of humidity;
  Serial.print('.');
  Serial.print(dat[1], DEC); //Displays the decimal places of the humidity;
  Serial.println('%');
  Serial.print("Temperature = ");
  Serial.print(dat[2], DEC); //Displays the integer bits of temperature;
  Serial.print('.');
  Serial.print(dat[3], DEC); //Displays the decimal places of the temperature;
  Serial.println('C');*/
  
}
