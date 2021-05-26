# Smokem

Smokem is a simple flexible program for using an M5Paper device to control an electric smoker (or any other cooking device that has a simple on/off heating element)

## Disclaimer

Wiring something that can burn your house down to an IoT device is a stupid idea. I am not responsible. Especially not if you do something stupid like burn your house down. But also in general. I AM NOT RESPONSIBLE!

## Bill of Materials

- [M5Paper](https://shop.m5stack.com/products/m5paper-esp32-development-kit-960x540-4-7-eink-display-235-ppi?variant=37595977908396)
- [SSR40-DA](https://cdn.sparkfun.com/datasheets/Components/General/SSR40DA.pdf) or any similar enough relay to control the AC hearting element from our DC M5
- [MAX6675](https://www.maximintegrated.com/en/products/sensors/MAX6675.html) for measuring the temperature in your target (for the smoker design use a sensor inside the smoker)
- Cables to wire it all up I guess.

## Why?

Because most temperature contollers meant to do this suck to program and I have an M5 Paper.

## TODO

- Actually test the dang thing
- Add webserver functionality so it can be set/checked from a computer
- Add some metrics to keep track of run time
