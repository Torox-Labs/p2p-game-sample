@sampler base_map "newmtl Cap"
@sampler glass_map "newmtl Glass" 
@sampler material_map "newmtl Material"
@sampler material_001_map "newmtl Material.001"
@sampler material_003_map "newmtl Material.003"
@sampler wing_map "newmtl Wing"

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
uniform mat3 rox_NormalMatrix;



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
    outNormal = normalize(rox_NormalMatrix * rox_Normal);
    
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

uniform sampler2D base_map;
uniform sampler2D glass_map;
uniform sampler2D material_map;
uniform sampler2D material_001_map;
uniform sampler2D material_003_map;
uniform sampler2D wing_map;

uniform vec4 camera_pos;
uniform vec4 dir;

out vec4 FragColor;

// Material properties from space_ship.mtl
struct Material {
    vec3 Ka;  // Ambient
    vec3 Kd;  // Diffuse
    vec3 Ks;  // Specular
    vec3 Ke;  // Emission
    float Ns; // Specular exponent
    float d;  // Dissolve (transparency)
    float Ni; // Index of refraction
};

Material getMaterial(int materialId) {
    Material mat;
    
    if (materialId == 0) { // Cap
        mat.Ka = vec3(1.0, 1.0, 1.0);
        mat.Kd = vec3(0.164193, 0.159245, 0.800000);
        mat.Ks = vec3(0.5, 0.5, 0.5);
        mat.Ke = vec3(0.0, 0.0, 0.0);
        mat.Ns = 250.0;
        mat.d = 1.0;
        mat.Ni = 1.0;
    } else if (materialId == 1) { // Glass
        mat.Ka = vec3(1.0, 1.0, 1.0);
        mat.Kd = vec3(0.494990, 0.618285, 0.800000);
        mat.Ks = vec3(0.5, 0.5, 0.5);
        mat.Ke = vec3(0.0, 0.0, 0.0);
        mat.Ns = 250.0;
        mat.d = 1.0;
        mat.Ni = 1.0;
    } else if (materialId == 2) { // Material
        mat.Ka = vec3(1.0, 1.0, 1.0);
        mat.Kd = vec3(0.8, 0.8, 0.8);
        mat.Ks = vec3(0.5, 0.5, 0.5);
        mat.Ke = vec3(0.0, 0.0, 0.0);
        mat.Ns = 250.0;
        mat.d = 1.0;
        mat.Ni = 1.0;
    } else if (materialId == 3) { // Material.001 (dark)
        mat.Ka = vec3(1.0, 1.0, 1.0);
        mat.Kd = vec3(0.072272, 0.074214, 0.084376);
        mat.Ks = vec3(0.5, 0.5, 0.5);
        mat.Ke = vec3(0.0, 0.0, 0.0);
        mat.Ns = 250.0;
        mat.d = 1.0;
        mat.Ni = 1.0;
    } else if (materialId == 4) { // Material.003 (red)
        mat.Ka = vec3(1.0, 1.0, 1.0);
        mat.Kd = vec3(1.0, 0.033105, 0.033105);
        mat.Ks = vec3(0.5, 0.5, 0.5);
        mat.Ke = vec3(0.0, 0.0, 0.0);
        mat.Ns = 250.0;
        mat.d = 1.0;
        mat.Ni = 1.0;
    } else { // Wing (default)
        mat.Ka = vec3(1.0, 1.0, 1.0);
        mat.Kd = vec3(0.239553, 0.239553, 0.239553);
        mat.Ks = vec3(0.5, 0.5, 0.5);
        mat.Ke = vec3(0.0, 0.0, 0.0);
        mat.Ns = 250.0;
        mat.d = 1.0;
        mat.Ni = 1.0;
    }
    
    return mat;
}

vec3 calculateLighting(Material mat, vec3 normal, vec3 lightDir, vec3 viewDir) {
    // Normalize vectors
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);
    vec3 R = reflect(-L, N);
    
    // Calculate diffuse
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = mat.Kd * NdotL;
    
    // Calculate specular
    float RdotV = max(dot(R, V), 0.0);
    vec3 specular = mat.Ks * pow(RdotV, mat.Ns);
    
    // Ambient
    vec3 ambient = mat.Ka * 0.2; // Simple ambient
    
    return ambient + diffuse + specular + mat.Ke;
}

void main()
{
    // Determine material based on group (this is a simplified approach)
    // In a real implementation, you'd get this from vertex attributes or uniforms
    int materialId = 0; // Default to Cap material
    
    // Simple heuristic based on vertex position or color to determine material
    // You may need to adjust this based on how your engine passes material info
    if (outColor.r > 0.8 && outColor.g < 0.1 && outColor.b < 0.1) {
        materialId = 4; // Red material
    } else if (outColor.r < 0.2 && outColor.g < 0.2 && outColor.b < 0.2) {
        materialId = 3; // Dark material
    } else if (outColor.b > 0.6) {
        materialId = 0; // Blue cap material
    }
    
    Material mat = getMaterial(materialId);
    
    // Simple lighting setup
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // Simple directional light
    vec3 viewDir = normalize(camera_pos.xyz - outWorldPos);
    
    // Calculate lighting
    vec3 finalColor = calculateLighting(mat, outNormal, lightDir, viewDir);
    
    // Apply transparency
    FragColor = vec4(finalColor, mat.d);
    
    // Optional: Sample texture if available (most materials don't have textures in your MTL)
    // vec4 texColor = texture2D(base_map, outTexCoord);
    // FragColor *= texColor;
}