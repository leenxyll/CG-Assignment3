#version 330 // เลขเดียวกับเวอร์ชั่น OpenGL

layout (location = 0) in vec3 pos; //vec3 = vector3 in = รับเข้ามาจากที่อื่น pos = ตัวแปร Postion

uniform mat4 model; //uniform ทำให้ตัวแปรใช้ได้กับทุกที่ 
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0); // vector4 คือเพิ่มแกน เหมือนตอนก่อนมิดเทอม แคสได้เลย ถ้าง่ายๆคือ vec4(pos, 1.0)
}