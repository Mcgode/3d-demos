

#define MAX_AMOUNT_OF_POINT_LIGHTS 4
struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    int enabled;

    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct Material {
    float shininess;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    sampler2D diffuseMap;
    sampler2D normalMap;
};


#define MAX_AMOUNT_OF_SHADOW_MAPS 5
struct ShadowMap {
    sampler2D shadowMap;
    mat4 lightSpaceMatrix;
    float shadowBias;
    float radius;
};


out vec4 FragColor;

in VS_OUT {
    vec3 worldPosition;
    vec2 textureCoordinates;
    vec3 tangeantWorldPosition;
    vec3 directionalLightDirection;
    vec3 cameraPosition;
    vec3 pointLightsPositions[MAX_AMOUNT_OF_POINT_LIGHTS];
    vec4 lightSpaceDirectionalPositions[MAX_AMOUNT_OF_SHADOW_MAPS];
} fs_in;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform int amountOfPointLights = 0;
uniform PointLight pointLights[MAX_AMOUNT_OF_POINT_LIGHTS];
uniform int amountOfShadowMaps = 0;
uniform ShadowMap directionalShadowMaps[MAX_AMOUNT_OF_SHADOW_MAPS];


float getShadow(ShadowMap shadowMap, vec4 position, int index)
{
    vec3 shadowCoords = position.xyz / position.w;
    if (abs(shadowCoords.x) > 1.0 || abs(shadowCoords.y) > 1.0)
    return 1.0;

    vec3 depthCoords = shadowCoords * 0.5 + 0.5;

    float depth = texture2D(shadowMap.shadowMap, depthCoords.xy).r;
    if (depth == 0.0 || depth == 1.0)
        return 1.0;

    for (int i = index - 1; i >= 0; i--) {
        float scaledDepth = shadowMap.radius * (depth + shadowMap.shadowBias - 0.5) / directionalShadowMaps[i].radius * 2.;
        vec2 scaledCoords = shadowCoords.xy * shadowMap.radius / directionalShadowMaps[i].radius;
        if (abs(scaledDepth) < 1.0 - directionalShadowMaps[i].shadowBias && abs(scaledCoords.x) <= 1.0 && abs(scaledCoords.y) <= 1.0) {
            return 1.0;
        }
    }

    return depth > depthCoords.z - shadowMap.shadowBias ? 1.0 : 0.0;
}


vec3 getPointLightColor(PointLight light, vec3 position, vec3 normal)
{
    vec3 norm = normal;
    vec3 l = normalize(position - fs_in.tangeantWorldPosition);
    float dist = length(fs_in.worldPosition - light.position);
    float distFactor = 1 / (light.constant + light.linear * dist + light.quadratic * pow(dist, 2));

    vec3 color = texture(material.diffuseMap, fs_in.textureCoordinates).xyz;

    vec3 ambientColor = color * material.ambient * light.ambient;

    float difFactor = max(0, dot(norm, l));
    vec3 diffuseColor = distFactor * difFactor * color * material.diffuse * light.diffuse;

    float shininess = material.shininess;
    vec3 r = reflect(l, norm);
    vec3 v = normalize(fs_in.cameraPosition - fs_in.tangeantWorldPosition);
    float specFactor = pow(max(0, dot(r, v)), shininess);
    vec3 specularColor = specFactor * light.specular * color * material.specular;

    return ambientColor + diffuseColor + specularColor;
}


vec3 getDirectionalLightColor(DirectionalLight light, vec3 direction, vec3 normal)
{
    vec3 color = texture(material.diffuseMap, fs_in.textureCoordinates).xyz;

    vec3 ambientColor = color * material.ambient * light.ambient;

    vec3 l = normalize(-direction);
    float difFactor = max(0, dot(l, normalize(normal)));
    vec3 diffuseColor = difFactor * color * material.diffuse * light.diffuse;

    float shininess = material.shininess;
    vec3 r = reflect(-l, normalize(normal));
    vec3 v = normalize(fs_in.cameraPosition - fs_in.tangeantWorldPosition);
    float specFactor = pow(max(0, dot(r, v)), shininess);
    vec3 specularColor = specFactor * light.specular * color * material.specular;

    float shadow = 1.0;
    for (int i = 0; i < amountOfShadowMaps; i++) {
        shadow *= getShadow(directionalShadowMaps[i], fs_in.lightSpaceDirectionalPositions[i], i);
        if (shadow < 1.0)
            break;
    }

    return ambientColor + shadow * (diffuseColor + specularColor);
}


void main() {
    vec3 normal = texture(material.normalMap, fs_in.textureCoordinates).rgb;
    normal = normalize(normal * 2.0 - 1.0);

	vec3 finalColor = vec3(0.0);

	if (directionalLight.enabled != 0)
	    finalColor += getDirectionalLightColor(directionalLight, fs_in.directionalLightDirection, normal);
	else
	    finalColor = texture(material.diffuseMap, fs_in.textureCoordinates).rgb;

	for (int i = 0; i < amountOfPointLights; i++)
	    finalColor += getPointLightColor(pointLights[i], fs_in.pointLightsPositions[i], normal);

	FragColor = vec4(finalColor, 1);
}
