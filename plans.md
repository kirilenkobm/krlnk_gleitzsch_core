# Future ideas

## Multiple entry points

Creating multiple entry points could be useful if you want to offer both simple and advanced customization options.
For example, one function could use preset filter names for ease of use,
while another could allow for detailed customization via the equalization curve.

## Optional arguments

- get rid of the `chunk_size` arg?
- get rid of the `size` arg?

At least, probably no need to make them mandatory.
Instead of filter name, apply something like:

~~Equalizer for both the real and imaginary part.~~

~~## Equalization Curve Idea~~

~~I am considering replacing the filter name argument with an "equalization curve", represented as a list of doubles~~
~~for both the real and imaginary parts of the frequency domain.~~
~~This curve would act as a set of multipliers for different frequency bands in the Fourier-transformed image data.~~

~~For example, if the equalization curve is [1.0, 0.0], it means that the lowest frequency components are left unchanged~~
~~(multiplied by 1.0), while the highest frequency components are nullified (multiplied by 0.0).~~
~~For frequencies in between, the multiplier would linearly interpolate between 1.0 and 0.0.~~

~~A more complex curve like [1.0, 0.0, 0.0, 0.5, 2.0] would apply a more nuanced transformation:~~

~~* The first 25% of frequencies would linearly decrease to zero.~~
~~* The next 25% would stay at zero.~~
~~* The following 25% would linearly increase to 0.5.~~
~~* The final frequencies would be amplified by a factor of 2.~~

Implemented.

