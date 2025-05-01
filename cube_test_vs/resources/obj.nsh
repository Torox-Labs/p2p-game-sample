@sampler base_map "map_Kd"

@all
//varying vec2 tc;
varying vec4 outColor;

@vertex

void main()
{
    outColor = vec4(0.5, 0.5, 0, 1.0);

    //tc = gl_MultiTexCoord0.xy;
    gl_Position = (gl_ModelViewProjectionMatrix * gl_Vertex) * 0.5;
}

@fragment
// uniform sampler2D base_map;

void main()
{
    //gl_FragColor = texture2D(base_map, tc);
    gl_FragColor = outColor;
}
