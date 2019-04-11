step 1:
	initial GPIO/Input device/ISR,there are two mode of touch interrupt as follows:
	(a)touch mode ->touch event up or dowm interrupt 
	(b)ADC mode ->ADC transform  done event interrupt
step 2:
	when touch event,into up_down ISR,change mode to ADC mode
step 3:
	when ADC transform done,into ADC transform done ISR
get ADC data,continue to start the ADC if the number of conversions is less than 4

step 4:
	when ADC transform 4 times,change mode to touch mode and star timer which debounce delay 5ms to report data 
step 5:
	After the data is reported,judged whether there is still a touch event, 
then the change to ADC mode and return to the third step