@sampler base_map "Cap"

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
    outNormal = normalize(normal_matrix * rox_Normal);
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

void main()
{
    // Cap material - Blue from space_ship.mtl
    vec3 materialColor = vec3(0.164193, 0.159245, 0.800000);
    
    vec3 N = normalize(outNormal);
    vec3 lightPos = vec3(2.0, 2.0, 2.0);
    vec3 lightDir = normalize(lightPos - outWorldPos);
    vec3 viewDir = normalize(camera_pos.xyz - outWorldPos);
    
    // Ambient
    vec3 ambient = materialColor * 0.4;
    
    // Diffuse
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = materialColor * diff * 0.7;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, N);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(0.5) * spec * 0.3;
    
    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor, 1.0);
}