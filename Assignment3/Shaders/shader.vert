#version 330 // เลขเดียวกับเวอร์ชั่น OpenGL

layout (location = 0) in vec3 pos; //vec3 = vector3 in = รับเข้ามาจากที่อื่น pos = ตัวแปร Postion
layout (location = 1) in vec2 aTexCood; 
layout (location = 2) in vec3 aNormal;

out vec4 vCol; //vColour
out vec2 TexCood;
out vec3 Normal;
out vec3 FragPos; //world

uniform mat4 model; //uniform ทำให้ตัวแปรใช้ได้กับทุกที่ 
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0); // vector4 คือเพิ่มแกน เหมือนตอนก่อนมิดเทอม แคสได้เลย ถ้าง่ายๆคือ vec4(pos, 1.0)
    FragPos = vec3(model * vec4(pos, 1.0));
    vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f); // clamp = ตั้งค่า ต่ำสุด มากสุด
    TexCood = aTexCood; //ส่งให้ frag
    Normal = aNormal;
}