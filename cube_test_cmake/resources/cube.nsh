@sampler base_map "map_Kd"

@all
varying vec2 tc;
varying vec4 color;
varying vec3 normal;
varying vec3 worldPos;

@vertex

out vec2 outTexCoord;
out vec4 outColor;
out vec3 outNormal;
out vec3 outWorldPos;

void main()
{
    // Transform vertex to world space
    vec4 worldVertex = rox_ModelViewMatrix * rox_Vertex;
    outWorldPos = worldVertex.xyz;
    
    // Transform normal to world space
    outNormal = normalize(rox_Normal);
    
    // Pass through texture coordinates and vertex color
    outTexCoord = rox_MultiTexCoord.xy;
    outColor = rox_Vertex;
    
    // Apply scaling and final position
    gl_Position = rox_ModelViewMatrix * worldVertex * 0.2;
}

@fragment

in vec2 outTexCoord;
in vec4 outColor;
in vec3 outNormal;
in vec3 outWorldPos;

uniform sampler2D base_map;
uniform vec4 camera_pos;

out vec4 FragColor;

// Simple lighting calculation
vec3 calculateLighting(vec3 baseColor, vec3 normal, vec3 worldPos) {
    // Normalize the normal
    vec3 N = normalize(normal);
    
    // Light setup
    vec3 lightPos = vec3(2.0, 2.0, 2.0); // Light position
    vec3 lightColor = vec3(1.0, 1.0, 1.0); // White light
    
    // Calculate light direction
    vec3 lightDir = normalize(lightPos - worldPos);
    
    // Calculate view direction
    vec3 viewDir = normalize(camera_pos.xyz - worldPos);
    
    // Ambient lighting
    vec3 ambient = baseColor * 0.3;
    
    // Diffuse lighting
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = baseColor * diff * lightColor * 0.8;
    
    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, N);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = lightColor * spec * 0.5;
    
    return ambient + diffuse + specular;
}

void main()
{
    // Sample the base texture
    vec4 texColor = texture2D(base_map, outTexCoord);
    
    // If no texture is available, use a default white color
    if (texColor.a < 0.1) {
        texColor = vec4(0.8, 0.8, 0.8, 1.0); // Light gray default
    }
    
    // Calculate lighting
    vec3 litColor = calculateLighting(texColor.rgb, outNormal, outWorldPos);
    
    // Output final color
    //FragColor = vec4(litColor, texColor.a);
    
    FragColor = vec4(outColor.xyz, texture2D(base_map, texColor.xy).r * outColor.a);

}