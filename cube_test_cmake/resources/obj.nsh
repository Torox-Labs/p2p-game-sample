@predefined camera_pos "nya camera pos"
@sampler base "diffuse"
@uniform param "param"
@uniform dir "dir"
@uniform pos "pos"

@all
varying vec2 tc;
varying vec4 color;
varying vec3 normal;
varying vec3 worldPos;


@vertex

uniform vec4 param[16];
uniform vec4 camera_pos;
uniform vec4 pos;
uniform vec4 dir;
uniform mat3 normal_matrix;

out vec2 outTexCoord;
out vec4 outColor;
out vec3 outNormal;
out vec3 outWorldPos;

void main()
{
    vec4 worldVertex = rox_ModelViewMatrix * rox_Vertex;
    outWorldPos = worldVertex.xyz;
    outNormal = normalize(rox_NormalMatrix * rox_Normal);
    outTexCoord = rox_MultiTexCoord.xy;
    outColor = rox_Vertex;
    gl_Position = rox_ProjectionMatrix * worldVertex;
}

@fragment

in vec2 outTexCoord;
in vec4 outColor;
in vec3 outNormal;
in vec3 outWorldPos;

uniform vec4 camera_pos;
uniform vec4 dir;

out vec4 FragColor;

// Simple lighting calculation
vec3 calculateLighting(vec3 baseColor, vec3 normal, vec3 worldPos) {
    vec3 N = normalize(normal);
    vec3 lightPos = vec3(2.0, 2.0, 2.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightDir = normalize(lightPos - worldPos);
    vec3 viewDir = normalize(camera_pos.xyz - worldPos);
    
    // Ambient lighting
    vec3 ambient = baseColor * 0.4;
    
    // Diffuse lighting
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = baseColor * diff * lightColor * 0.7;
    
    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, N);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = lightColor * spec * 0.3;
    
    return ambient + diffuse + specular;
}

void main()
{
    // Use world position hash to determine material type
    // This is a workaround since we can't directly get material ID
    vec3 pos = outWorldPos;
    vec3 norm = normalize(outNormal);
    
    // Create a pseudo-random value from world position
    float hash = sin(pos.x * 12.9898 + pos.y * 78.233 + pos.z * 37.719) * 43758.5453;
    hash = fract(abs(hash));
    
    vec3 materialColor;
    
    // Use normal directions and position to approximate material assignment
    if (norm.z > 0.6) {
        // Top faces - Cap (Blue)
        materialColor = vec3(0.164193, 0.159245, 0.800000);
    } else if (norm.z < -0.6) {
        // Bottom faces - Material.003 (Red)  
        materialColor = vec3(1.0, 0.033105, 0.033105);
    } else if (abs(norm.x) > 0.7) {
        // Side faces X - Glass (Light Blue)
        materialColor = vec3(0.494990, 0.618285, 0.800000);
    } else if (abs(norm.y) > 0.7) {
        // Side faces Y - Material.001 (Dark Gray)
        materialColor = vec3(0.072272, 0.074214, 0.084376);
    } else if (hash > 0.6) {
        // Random distribution - Wing (Medium Gray)
        materialColor = vec3(0.239553, 0.239553, 0.239553);
    } else {
        // Default - Material (Light Gray)
        materialColor = vec3(0.8, 0.8, 0.8);
    }
    
    vec3 litColor = calculateLighting(materialColor, outNormal, outWorldPos);
    FragColor = vec4(litColor, 1.0);
}