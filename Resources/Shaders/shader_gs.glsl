#version 330

layout(points) in;
layout(line_strip, max_vertices = 256) out;

uniform mat4 gMVP;

void main()
{
	float PI = 3.14159;

	for(float i = 0; i <= 1.0001; i += 0.005)
	{
		float ang;
		if(i ==0)
		{
			ang = 0;
		}
		else
		{
			ang = (PI * 2) * i;
		}

		gl_Position = gMVP * (gl_in[0].gl_Position + vec4(0.5 * sin(ang), 0.5 * cos(ang), 0.0, 0.0));
		EmitVertex();

	}

	gl_Position = gl_in[0].gl_Position + vec4(-0.5, -0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.5, -0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.5, 0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.5, 0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.5, -0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.5, 0.0, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.5, 0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.0, -0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.5, 0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.5, 0.0, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.5, -0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.5, 0.0, 0.0);
	gl_Position = gMVP * gl_Position;
    EmitVertex();

    EndPrimitive();
}
