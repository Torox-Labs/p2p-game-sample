@all
<<<<<<< HEAD
varying vec4 outColor;
=======
>>>>>>> 756c6f87d4accfcfa075a5f567820ab6f93b41d2

@vertex

out vec2 outTexCoord;
out vec4 outColor;

void main()
{
<<<<<<< HEAD
    outColor = vec4(1.0f, 1.0f, 0, 1.0f);
    gl_Position = rox_ModelViewMatrix * rox_Vertex;
=======
    outColor = rox_Vertex;
    outTexCoord = rox_MultiTexCoord.xy;
	gl_Position = rox_ModelViewMatrix * rox_Vertex * 0.2f;
>>>>>>> 756c6f87d4accfcfa075a5f567820ab6f93b41d2
}

@fragment

in vec2 outTexCoord;
in vec4 outColor;

uniform sampler2D base_map;

void main()
{
<<<<<<< HEAD
    rox_FragColor = outColor;
=======
    FragColor = texture2D(base_map, outTexCoord);
>>>>>>> 756c6f87d4accfcfa075a5f567820ab6f93b41d2
}
