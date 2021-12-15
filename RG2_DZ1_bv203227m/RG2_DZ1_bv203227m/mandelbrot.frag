#version 460 core

uniform int iterations;
uniform double zoom;
uniform dvec2 screenSize;
uniform dvec2 offset;

double n = 0.0;
double thresholdMagnitude = 2.0;

double mandelbrot(dvec2 c) {
	dvec2 z = vec2(0.0, 0.0);
	for (int i = 0; i < iterations; i++) {
		dvec2 zn1;
		zn1.x = (z.x * z.x) - (z.y * z.y) + c.x;
		zn1.y = (2.0 * z.x * z.y) + c.y;
		z = zn1;
		if ((z.x * z.x) + (z.y * z.y) > thresholdMagnitude) {
			break;
		}
		n++;
	}
	return n / float(iterations);
}

vec4 mandelbrotToRGBDefault(float t) {
	float r = 9.0 * (1.0 - t) * t * t * t;
	float g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
	float b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;
	return vec4(r, g, b, 1.0);
}

// All components are in the range [0…1], including hue
vec3 hsvToRGB(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 mandelbrotToHSVToRGB(float m) {
	vec3 hsv = vec3(m, 1.0, 1.0);
	vec3 rgb = hsvToRGB(hsv);
	return vec4(rgb.r, rgb.g, rgb.b, 1.0);
}

void main() {
	dvec2 coord = dvec2(gl_FragCoord.xy);
	double t = mandelbrot(((coord - screenSize/2) / zoom) - offset);
	if (gl_FragCoord.x < 40) {
		gl_FragColor = vec4(1.0);
	}
	//gl_FragColor = mandelbrotToRGBDefault(float(t));
	gl_FragColor = mandelbrotToHSVToRGB(float(t));
}