# ESP_fast_reading
This is the Sketch to compaire reading speed for SPIFF impelemntation versus flash raw reading 

webserver for uploading images or file implemented so after connecting to wifi , can go to IP address and upload the images

for testing this sketch 3 file has been added in this repo, although any file with size greater than 80 KB would be good 
. Each file consist the 200 image lines, and each line consisit RGB color data for  144 LED, so each reading line equal 432 bytes
, ``` uint8_t  * LED_BUFFER; ``` considered for this amount of data , when this buffer filled then inserte to LED show function 
for  compile this sketch also need FastLED repo [https://github.com/FastLED/FastLED]
A ESP-12F module has been choosing for this test and set the flash module configuration set to (2M , 1M SPIFF)
if the CPU speed and Flash speed multiplied the raw reading increase around 2 ms
