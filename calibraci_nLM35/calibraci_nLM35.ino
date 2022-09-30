#include <GraphSeries.h>
GraphSeries g_aGraphs[] = {"temperatura ambiente"};
//////////////////////////// for_Sensors//////////////////////////////
enum sensors {temperature, humidity, zone1y2, zone3y4, allzones};
void setup() {
  Serial.begin(9600);
}

void loop() {
  float temp=(analogRead(0)*5.0/1024.0)*100.0;
  g_aGraphs[0].SendData(temp);
}

////////exponential_filter(valor actual,valor anterior,facor de correción);
float exponential_filter(float Xk, float * Yk_1, float a)
{
  float Yk = *Yk_1 + a * (Xk - *Yk_1);
  return Yk;
}
