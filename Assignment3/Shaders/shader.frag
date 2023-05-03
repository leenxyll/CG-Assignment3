#version 330

in vec4 vCol; // ชื่อเดียวกับใน Vert
out vec4 colour; //out = ส่งออกไปที่ framebuffer
in vec2 TexCood;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture2D;
uniform vec3 lightColour;

vec3 ambientLight(){ //แสงจากดวงอาทิตย์
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColour;
    return ambient;
}

vec3 diffuseLigth(){ //ไม่ขึ้นอยู่กับคนมอง แสงจากไฟฉาย คนมองย้ายแสดงนี้ก็อยู่ตำแหน่งเดิม //Lambert ใช้กับวัตถุเรียบๆ
    float diffuseStrength = 0.1f;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0); //สลับตำแหน่งได้
    // if(diff < 0) diff = 0;
    vec3 diffuse = diffuseStrength * lightColour * diff;
    return diffuse;
}

vec3 specularLight(){ //ขึ้นอยู่กับคนมอง ex กระจก
    float specularStrength = 0.5f;
    float shininess = 10.0f; //ยิ่งเพิ่มเส้นยิ่งเล็กลงยิ่งสว่างขึ้น ex เหล็ก สูงๆ / พลาสติก ต่ำๆ แล้วเลือกสีเอา
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 reflectDir = reflect(-lightDir, norm);
    //Phong
    // float spec = pow(max(dot(reflectDir, viewDir), 0), shininess); 

    //Blinn-Phong แก้ปัญหาองศา reflect กับ View ที่เกิน 90 จะตัดเป็นศูนย์
    vec3 halfDir = (lightDir + viewDir) / 2.0f;
    float spec = pow(max(dot(halfDir, norm), 0), shininess); 


    vec3 specular = specularStrength * lightColour * spec;
    return specular;
}

void main()
{
    // colour = texture(texture2D, TexCood) * vec4(ambientLight() + diffuseLigth() , 1.0f);
    // colour = texture(texture2D, TexCood) * vec4(/*ambientLight() + */ specularLight() , 1.0f);
    colour = texture(texture2D, TexCood) * vec4(ambientLight() + diffuseLigth() + specularLight() , 1.0f);
}