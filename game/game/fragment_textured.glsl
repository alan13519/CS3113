
uniform sampler2D diffuse;
varying vec2 texCoordVar;

uniform float alphaValue;

void main() {
	gl_FragColor = texture2D(diffuse, texCoordVar);
}