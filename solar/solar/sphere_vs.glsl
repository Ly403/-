#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 normal; //������
layout (location = 3) in vec3 aTexCoord;
out vec3 ourColor;
out vec3 normalIn;
out vec3 fragPosIn;
out vec2 texCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	ourColor = aColor;
	fragPosIn = vec3(model * vec4(aPos, 1.0f));
    normalIn = mat3(transpose(inverse(model))) * normal;
	texCoord = vec2(aTexCoord.x, 1.0f - aTexCoord.y);
};