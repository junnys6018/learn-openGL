#ifndef TEST_CUBE_MAP
#define TEST_CUBE_MAP

#include "test.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"

#include <memory>

#include "imgui-master/imgui.h"
#include "STB_IMAGE/stb_image.h"

class TestCubeMap : public Test
{
public:
	TestCubeMap(Camera& cam, GLFWwindow* win);
	~TestCubeMap();

	void OnUpdate() override;
	void OnImGuiRender() override;
private:
	void loadCubeMap(std::vector<std::string> faces);
	Camera& m_camera;
	GLFWwindow* m_window;

	std::unique_ptr<Object> oBlastoise;

	std::unique_ptr<Shader> skyBoxShader;
	std::unique_ptr<Shader> m_shader;

	std::unique_ptr<VertexBuffer> skyBoxVB;
	std::unique_ptr<VertexArray> skyBoxVA;
	
	unsigned int m_cubeMap;

};


#endif 
