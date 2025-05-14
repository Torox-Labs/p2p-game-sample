@sampler base_map "map_Kd"

@all

@vertex

out vec2 outTexCoord;
out vec4 outColor;

void main()
{
    outColor = rox_Vertex;
    outTexCoord = rox_MultiTexCoord.xy;
	gl_Position = rox_ModelViewMatrix * rox_Vertex * 0.2f;
}

@fragment
varying vec4 FragColor;

in vec2 outTexCoord;
in vec4 outColor;

uniform sampler2D base_map;

void main()
{
    FragColor = texture2D(base_map, outTexCoord);
}
