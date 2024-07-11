A simulator of the "Pineapple" problem from IWAA 2024.

# Problem statement

Consider a desert planet with radius R = 6 000 km whose equatorial plane has zero inclination relative to its orbital plane
and whose synodic rotation period (solar day) is P = 24 hours. Starting at a latitude φ (where 0◦ < φ < 90◦), Marco
decided to set out on a tour of his life: every morning, he wakes up at sunrise and starts walking at constant speed
v = 5 km/h. He continuously changes his direction so that he always follows the azimuth of the Sun. At sunset he goes to
sleep. And then he repeats, every day. Find the change of φ in Marco’s latitude per one solar day as a function of φ. Can
he make it to the equator in finite time?

## Solutions
As the problem is stated, you do not reach the equator in finite time -- solving the geometry gives you an integral form for the latitute
as a function of time. This integral can be computed, but does not converge (but just slightly). This was calculated analytically by J.V.

## Generalized problem
What if your walking speed is quite large or the axial tilt is non-zero, one starts obtaining complicated paths on the Earth.
The original solution suggested that the daily shift in longitude is zero. This does not hold.


This simulator allows to explore the entire parameter space, i.e., different speeds, axial tilts, etc., witout the necessity for simple analytical models.
