@sampler base_map "map_Kd"

@uniform param "param"
@uniform dir "dir"
@uniform pos "pos"

@all
varying vec2 tc;
varying vec4 color;

@vertex

uniform vec4 param[16];
uniform vec4 camera_pos;
uniform vec4 pos;
uniform vec4 dir;

out vec2 outTexCoord;
out vec4 outColor;

void main()
{

    outColor = rox_Vertex;
    outTexCoord = rox_MultiTexCoord.xy;
	gl_Position = rox_ModelViewMatrix * rox_Vertex;
}

@fragment
varying vec4 FragColor;

in vec2 outTexCoord;
in vec4 outColor;

uniform sampler2D base_map;

void main()
{
    // FragColor = texture2D(base_map, outTexCoord);
    FragColor = outColor;

}
