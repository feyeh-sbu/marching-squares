#version 450 core

layout (location = 0) in vec2 pos;
layout (location = 1) in float intensity;

out vec4 color;

uniform float window_width;
uniform float window_height;

void main()
{
	gl_Position = vec4(2.0f * pos.x / window_width  - 1.0f,
					   2.0f * pos.y / window_height - 1.0f,
					   0.0f, 1.0f);

	float value = intensity + 0.5f;
	color = vec4(value, value, value, 1.0f);
}