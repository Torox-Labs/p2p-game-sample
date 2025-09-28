@sampler base "Glass"
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
    // Transform vertex to world space
    vec4 worldVertex = rox_ModelViewMatrix * rox_Vertex;
    outWorldPos = worldVertex.xyz;
    
    // Transform normal to world space
    outNormal = normalize(normal_matrix * rox_Normal);
    
    // Pass through texture coordinates and vertex color
    outTexCoord = rox_MultiTexCoord.xy;
    outColor = rox_Vertex;
    
    // Final position
    gl_Position = rox_ModelViewMatrix * worldVertex;
}

@fragment

in vec2 outTexCoord;
in vec4 outColor;
in vec3 outNormal;
in vec3 outWorldPos;

uniform vec4 camera_pos;
uniform vec4 dir;

out vec4 FragColor;

// Simplified material properties
struct Material {
    vec3 Ka;  // Ambient
    vec3 Kd;  // Diffuse
    vec3 Ks;  // Specular
    float Ns; // Specular exponent
};

Material getMaterial(int materialId) {
    Material mat;
    
    if (materialId == 0) { // Cap - Blue
        mat.Ka = vec3(0.1, 0.1, 0.4);
        mat.Kd = vec3(0.164193, 0.159245, 0.800000);
        mat.Ks = vec3(0.3, 0.3, 0.3);
        mat.Ns = 32.0;
    } else if (materialId == 1) { // Glass - Light Blue
        mat.Ka = vec3(0.2, 0.3, 0.4);
        mat.Kd = vec3(0.494990, 0.618285, 0.800000);
        mat.Ks = vec3(0.3, 0.3, 0.3);
        mat.Ns = 32.0;
    } else if (materialId == 2) { // Material - Light Gray
        mat.Ka = vec3(0.3, 0.3, 0.3);
        mat.Kd = vec3(0.8, 0.8, 0.8);
        mat.Ks = vec3(0.2, 0.2, 0.2);
        mat.Ns = 16.0;
    } else if (materialId == 3) { // Material.001 - Dark Gray
        mat.Ka = vec3(0.05, 0.05, 0.05);
        mat.Kd = vec3(0.072272, 0.074214, 0.084376);
        mat.Ks = vec3(0.1, 0.1, 0.1);
        mat.Ns = 16.0;
    } else if (materialId == 4) { // Material.003 - Red
        mat.Ka = vec3(0.3, 0.1, 0.1);
        mat.Kd = vec3(1.0, 0.033105, 0.033105);
        mat.Ks = vec3(0.3, 0.1, 0.1);
        mat.Ns = 32.0;
    } else { // Wing - Medium Gray (default)
        mat.Ka = vec3(0.15, 0.15, 0.15);
        mat.Kd = vec3(0.239553, 0.239553, 0.239553);
        mat.Ks = vec3(0.15, 0.15, 0.15);
        mat.Ns = 16.0;
    }
    
    return mat;
}

// SIMPLIFIED lighting calculation like cube.nsh
vec3 calculateLighting(vec3 baseColor, vec3 normal, vec3 worldPos, vec3 specularColor, float shininess) {
    // Normalize the normal
    vec3 N = normalize(normal);
    
    // Light setup (similar to cube.nsh)
    vec3 lightPos = vec3(2.0, 2.0, 2.0); // Light position
    vec3 lightColor = vec3(1.0, 1.0, 1.0); // White light
    
    // Calculate light direction
    vec3 lightDir = normalize(lightPos - worldPos);
    
    // Calculate view direction
    vec3 viewDir = normalize(camera_pos.xyz - worldPos);
    
    // Ambient lighting (stronger than before)
    vec3 ambient = baseColor * 0.4;
    
    // Diffuse lighting
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = baseColor * diff * lightColor * 0.7;
    
    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, N);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularColor * spec * 0.3;
    
    return ambient + diffuse + specular;
}

void main()
{
    // SIMPLIFIED material selection
    int materialId = 2; // Default to light gray
    
    vec3 norm = normalize(outNormal);
    
    // Simple material selection based on normals
    if (norm.z > 0.7) {
        materialId = 0; // Blue - top faces
    } else if (norm.z < -0.7) {
        materialId = 4; // Red - bottom faces  
    } else if (abs(norm.x) > 0.7) {
        materialId = 1; // Light blue - side faces
    } else if (abs(norm.y) > 0.7) {
        materialId = 3; // Dark gray - front/back faces
    }
    // else use default gray (materialId = 2)
    
    Material mat = getMaterial(materialId);
    
    // SINGLE light calculation (like cube.nsh)
    vec3 finalColor = calculateLighting(mat.Kd, outNormal, outWorldPos, mat.Ks, mat.Ns);
    
    // Add ambient contribution
    finalColor += mat.Ka;
    
    FragColor = vec4(finalColor, 1.0);
}