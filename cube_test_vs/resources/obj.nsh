@all
varying vec4 outColor;

@vertex

void main()
{
    outColor = vec4(1.0f, 1.0f, 0, 1.0f);
    gl_Position = rox_ModelViewMatrix * rox_Vertex;
}

@fragment

void main()
{
    rox_FragColor = outColor;
}
