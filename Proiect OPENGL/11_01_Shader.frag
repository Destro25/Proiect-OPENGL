//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul XI - 11_01_Shader.frag |
// ======================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330 core
 
in vec3 FragPos;  
in vec3 Normal; 
in vec3 inLightPos;
in vec3 inViewPos;
in vec3 dir;
in vec3 ex_Color;
in vec2 tex_Coord;
in mat3 TBN; 
out vec4 out_Color;

uniform sampler2D iarba; 
uniform sampler2D iarbaNormal;
uniform sampler2D brickwall; 
uniform sampler2D brickwall_normal;
uniform vec3 lightColor;
uniform int codCol; 
float zDist; // Ex 2
vec3 GetNormalIarba(){
    return texture(iarbaNormal, tex_Coord).xyz * 2.0 - 1.0;
}

vec3 GetNormalBrickWall(){
    return texture(brickwall_normal, tex_Coord).xyz * 2.0 - 1.0;
}
void main(void)
  {
    if (codCol==0) // pentru codCol==0 este aplicata iluminarea
    {
  	    // Ambient
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * lightColor;
  	
        // Diffuse 
        vec3 normala = normalize(Normal);
        vec3 lightDir = normalize(inLightPos - FragPos);
        //vec3 lightDir = normalize(dir); // cazul unei surse directionale
        float diff = max(dot(normala, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
    
        // Specular
        float specularStrength = 0.5f;
        vec3 viewDir = normalize(inViewPos - FragPos);//vector catre observator normalizat (V)
        vec3 reflectDir = reflect(-lightDir, normala); // reflexia razei de lumina (R)
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
        vec3 specular = specularStrength * spec * lightColor;  
        vec3 emission = vec3(0.0, 0.0, 0.0);
        vec3 result = emission+(ambient + diffuse + specular) * ex_Color;
        vec3 fogColor = vec3(0.5f, 0.5f, 0.5f); // Ex 2
        zDist = length(inViewPos - FragPos); // Ex 2
        vec3 resultNew = mix(fogColor, result, exp(-0.005f * zDist)); // Ex 2
	    out_Color = vec4(resultNew, 1.0f); // Ex 2
        out_Color = vec4(result, 1.0f); // Pentru ceata, comenteaza linia asta
    }
    if (codCol==2) // pentru codCol==0 este aplicata iluminarea
    {
        vec4 textureColor = texture(iarba, tex_Coord);
  	    // Ambient
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * textureColor.rgb;
  	
        // Diffuse 
        vec3 normala = normalize(TBN * GetNormalIarba());
        //vec3 normala = normalize(Normal);
        vec3 lightDir = normalize(inLightPos - FragPos);
        //vec3 lightDir = normalize(dir); // cazul unei surse directionale
        float diff = max(dot(normala, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * textureColor.rgb;
    
        // Specular
        float specularStrength = 0.5f;
        vec3 viewDir = normalize(inViewPos - FragPos);//vector catre observator normalizat (V)
        vec3 reflectDir = reflect(-lightDir, normala); // reflexia razei de lumina (R)
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
        vec3 specular = specularStrength * spec * lightColor * textureColor.rgb;  
        vec3 emission = vec3(0.0, 0.0, 0.0);
        vec3 result = emission+(ambient + diffuse + specular) * textureColor.rgb;
        vec3 fogColor = vec3(0.5f, 0.5f, 0.5f); // Ex 2
        zDist = length(inViewPos - FragPos); // Ex 2
        vec3 resultNew = mix(fogColor, result, exp(-0.005f * zDist)); // Ex 2
	    out_Color = vec4(resultNew, 1.0f); // Ex 2
        out_Color = vec4(result, 1.0f); // Pentru ceata, comenteaza linia asta
    }

    if (codCol==1) // pentru codCol==1 este desenata umbra
    {
        vec3 black = vec3 (0.0, 0.0, 0.0);
        vec3 fogColor = vec3(0.5f, 0.5f, 0.5f); // Ex 2
        zDist = length(inViewPos - FragPos); // Ex 2
        vec3 resultNew = mix(fogColor, black, exp(-0.000015f * zDist)); // Ex 2
	    out_Color = vec4(resultNew, 1.0f); // Ex 2
		out_Color = vec4 (black, 1.0); // Pentru ceata, comenteaza linia asta

     }
     
     if (codCol==3) // pentru codCol==3 este pt textura de perete
    {
        vec4 textureColor = texture(brickwall, tex_Coord);
  	    // Ambient
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * textureColor.rgb;
  	
        // Diffuse 
        //vec3 normala = normalize(Normal);
        vec3 normala = normalize(TBN * GetNormalBrickWall());
        vec3 lightDir = normalize(inLightPos - FragPos);
        //vec3 lightDir = normalize(dir); // cazul unei surse directionale
        float diff = max(dot(normala, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * textureColor.rgb;
    
        // Specular
        float specularStrength = 0.5f;
        vec3 viewDir = normalize(inViewPos - FragPos);//vector catre observator normalizat (V)
        vec3 reflectDir = reflect(-lightDir, normala); // reflexia razei de lumina (R)
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
        vec3 specular = specularStrength * spec * lightColor * textureColor.rgb;  
        vec3 emission = vec3(0.0, 0.0, 0.0);
        vec3 result = emission+(ambient + diffuse + specular) * textureColor.rgb;
        vec3 fogColor = vec3(0.5f, 0.5f, 0.5f); // Ex 2
        zDist = length(inViewPos - FragPos); // Ex 2
        vec3 resultNew = mix(fogColor, result, exp(-0.005f * zDist)); // Ex 2
	    out_Color = vec4(resultNew, 1.0f); // Ex 2
        out_Color = vec4(result, 1.0f); // Pentru ceata, comenteaza linia asta

    }

    if (codCol==4) // pentru codCol==0 este aplicata iluminarea
    {
  	    // Ambient
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * lightColor;
  	
        // Diffuse 
        vec3 normala = normalize(Normal);
        vec3 lightDir = normalize(inLightPos - FragPos);
        //vec3 lightDir = normalize(dir); // cazul unei surse directionale
        float diff = max(dot(normala, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
    
        // Specular
        float specularStrength = 0.5f;
        vec3 viewDir = normalize(inViewPos - FragPos);//vector catre observator normalizat (V)
        vec3 reflectDir = reflect(-lightDir, normala); // reflexia razei de lumina (R)
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
        vec3 specular = specularStrength * spec * lightColor;  
        vec3 emission = vec3(0.0, 0.0, 0.0);
        vec3 result = emission+(ambient + diffuse + specular) * ex_Color;
        vec3 fogColor = vec3(0.5f, 0.5f, 0.5f); // Ex 2
        zDist = length(inViewPos - FragPos); // Ex 2
        vec3 resultNew = mix(fogColor, result, exp(-0.005f * zDist)); // Ex 2
	    out_Color = vec4(resultNew, 1.0f); // Ex 2
        out_Color = vec4(result, 1.0f); // Pentru ceata, comenteaza linia asta
    }
  }
