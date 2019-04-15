#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;
in vec3 fragPos;
out vec4 fColor;

//lighting
uniform	mat3 normalMatrix;
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor;
uniform	vec3 lightDir;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 64.0f;

vec3 o;
vec4 fogColor=vec4(0.5f,0.5f,0.5f,1.0f);
struct point{
	//Positia luminii
	vec3 position;

	//Constante ale formulii atenuarii luminii
	float constant;
	float linear;
	float quadratic;
	
	//Elemente ale culorii luminii
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform point lumina[2];
vec3 computePointLight(point lumina,vec3 normal,vec3 fragPos,vec3 viewDir)
{
	vec3 light=normalize(lumina.position-fragPos);

	float diffuseStrength=max(dot(light,normal),0.0f);

	vec3 reflectDir=reflect(-light,normal);
	float specularStrength=pow(max(dot(viewDir,reflectDir),0.0f),shininess);

	float distance=length(lumina.position-fragPos);
	float att=1.0/(lumina.constant+distance*lumina.linear+(distance*distance)*lumina.quadratic);

	vec3 ambient=lumina.ambient*vec3(texture(diffuseTexture,fragTexCoords));
	vec3 diffuse=lumina.diffuse*diffuseStrength*vec3(texture(diffuseTexture,fragTexCoords));
	vec3 specular=lumina.specular*specularStrength*vec3(texture(diffuseTexture,fragTexCoords));
	ambient*=att;
	diffuse*=att;
	specular*=att;
	return (ambient+diffuse+specular);
}
void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);	

	
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	

	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	
	ambient = ambientStrength * lightColor;
	

	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeShadow()
{	
	
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
   
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    

    float currentDepth = normalizedCoords.z;
    
    float bias = 0.005f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}
float computeFog()
{
	float fogDensity=0.10f;
	float fogFragment=length(fragPosEye);
	float fog=exp(-pow(fogDensity*fogFragment,2));
	return clamp(fog,0.0f,1.0f);
}
void main() 
{
	computeLightComponents();
	
	float shadow = computeShadow();
	
	//modulate with diffuse map
	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	//modulate woth specular map
	specular *= vec3(texture(specularTexture, fragTexCoords));
	
	//modulate with shadow
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	for(int i=0;i<2;i++)
		color+=computePointLight(lumina[i],normal,fragPosEye.xyz,normalize(-fragPosEye.xyz));
	float fogFactor=computeFog();
    fColor = fogColor*(1-fogFactor)+vec4(color, 1.0f)*fogFactor;
  //fColor=vec4(color,1.0f);
    //fColor = vec4(o, 1.0f);
}
