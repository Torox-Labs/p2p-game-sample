@sampler base_map "map_Kd"

@all
varying vec4 outColor;
varying vec4 FragColor;

@vertex

void main()
{
    outColor = vec4(0.5, 0.5, 0, 1.0);
    gl_Position=rox_ModelViewProjectionMatrix*rox_Vertex;
}

@fragment

void main()
{
    FragColor = outColor;
}
