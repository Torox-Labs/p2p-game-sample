#include <iostream>
#include <RoxApp/RoxApp.h>
#include <RoxRender/RoxVbo.h>
#include <RoxRender/RoxShader.h>
#include <RoxRender/RoxRender.h>
#include <RoxLogger/RoxLogger.h>
#include <RoxInput/RoxInput.h>
#include <RoxScene/location.h>
#include <RoxScene/camera.h>
#include <RoxScene/mesh.h>

#include <sstream>

class testCube : public RoxApp::RoxApp
{
private:
	bool onSplash() override
	{
		RoxLogger::log() << "Splash\n";

		RoxRender::setClearColor(1.0f, 0.5f, 0, 1.0f);
		RoxRender::clear(true, true);

		return true;
	}

	void testFileReading(const char* path)
	{
		FILE* file = nullptr;
		fopen_s(&file, path, "rb");

		if (!file)
		{
			std::cerr << "Failed to open file: " << path << std::endl;
			return;
		}

		fseek(file, 0, SEEK_END);
		size_t fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		std::cout << "File size: " << fileSize << " bytes." << std::endl;

		fclose(file);
	}

	void onInit() override
	{
		RoxLogger::log() << "Init\n";

		RoxRender::setClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		RoxRender::setClearDepth(1.0f);
		RoxRender::DepthTest::enable(RoxRender::DepthTest::LESS);

		RoxScene::mesh::register_load_function(RoxScene::mesh::load_nms);

		//testFileReading("D:/Dev/rox-engine-build/x64/Debug/new.nms");


		//bool boolValue = m_mesh.load("D:/Dev/rox-engine-build/x64/Debug/new.nms");
		//if (!boolValue)
		//{
		//	// Handle loading error
		//	std::cerr << "Failed to load NMS mesh file." << std::endl;
		//	return;
		//}

		float vertices[] =
		{
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f
		};

		unsigned short indices[] =
		{
			0, 2, 1, 1, 2, 3, // -x
			4, 5, 6, 5, 7, 6, // +x
			0, 1, 5, 0, 5, 4, // -y
			2, 6, 7, 2, 7, 3, // +y
			0, 4, 6, 0, 6, 2, // -z
			1, 3, 7, 1, 7, 5, // +z
		};

		m_vbo.setVertexData(vertices, sizeof(float) * 6, 8);
		m_vbo.setVertices(0, 3);
		m_vbo.setColors(sizeof(float) * 3, 3);
		m_vbo.setIndexData(indices, RoxRender::RoxVbo::INDEX_2D,
			sizeof(indices) / sizeof(unsigned short));
		

		const char* vs_code = R"(//#version 330 core		
								varying vec4 color;
								void main()
								{
								color=gl_Color;
								gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;
							})";

		const char* ps_code = R"(//#version 330 core
								varying vec4 color;
								void main()
								{
								gl_FragColor=color;
							})";

		m_shader.addProgram(RoxRender::RoxShader::VERTEX, vs_code);
		m_shader.addProgram(RoxRender::RoxShader::PIXEL, ps_code);
	}

	void onFrame(unsigned int dt) override
	{
		RoxRender::clear(true, true);

		m_rot += dt * 0.05f;
		if (m_rot > 360)
			m_rot = 0;

		RoxMath::Matrix4 mv;
		mv.translate(0, 0, -2.0f);
		mv.rotate(30.0f, 1.0f, 0.0f, 0.0f);
		mv.rotate(m_rot, 0.0f, 1.0f, 0.0f);
		
		m_camera.set_pos(m_move_x, m_move_y, m_move_z);
		m_camera.set_rot(m_rotate_x, m_rotate_y, m_rotate_z);
		//RoxRender::setModelviewMatrix(m_camera.get_view_matrix());

		RoxRender::setModelviewMatrix(mv);

		//RoxRender::setCamera(m_camera.get_view_matrix());

		m_shader.bind();
		m_vbo.bind();
		m_vbo.draw();
		m_vbo.unbind();
		m_shader.unbind();

		//m_mesh.draw();

		static unsigned int fps_counter = 0, fps_update_timer = 0;
		++fps_counter;
		fps_update_timer += dt;
		if (fps_update_timer > 1000)
		{
			std::ostringstream os;
			os << "Rox Engine Demo " << fps_counter << " fps";
			std::string str = os.str();
			setTitle(str.c_str());

			fps_update_timer %= 1000;
			fps_counter = 0;
		}
	}

	void onResize(unsigned int w, unsigned int h) override
	{
		RoxLogger::log() << "on_resize " << w << " " << h << "\n";

		if (!w || !h)
			return;

		RoxMath::Matrix4 proj;
		proj.perspective(70.0f, float(w) / h, 0.01f, 100.0f);
		RoxRender::setProjectionMatrix(proj);
	}

	void onFree() override
	{
		RoxLogger::log() << "on_free\n";

		m_vbo.release();
		m_shader.release();
	}

	void onKeyDown(unsigned int key, bool pressed) override
	{
		RoxLogger::log() << "key " << key << " " << pressed << "\n";

		if ((key == ::RoxInput::KEY_BACK || key == ::RoxInput::KEY_ESCAPE) && !pressed)
			finish();

		if(key == ::RoxInput::KEY_W && pressed)
		{
			RoxLogger::log() << "A pressed\n";
			m_move_z += 0.1f;
		}

		if (key == ::RoxInput::KEY_S && pressed)
		{
			RoxLogger::log() << "D pressed\n";
			m_move_z -= 0.1f;
		}

		if(key == ::RoxInput::KEY_SHIFT && pressed)
			if ((key == ::RoxInput::KEY_W) && pressed)
			{
				RoxLogger::log() << "Shift + A pressed\n";
				m_move_z += 10.0f;
			}
		
		if (key == ::RoxInput::KEY_SHIFT && pressed)
			if (key == ::RoxInput::KEY_S && key == ::RoxInput::KEY_SHIFT && pressed)
			{
				RoxLogger::log() << "Shift + D pressed\n";
				m_move_z -= 10.0f;
			}


		if (key == ::RoxInput::KEY_D && pressed)
		{
			RoxLogger::log() << "D pressed\n";
			m_move_x -= 0.1f; 
		}

		if (key == ::RoxInput::KEY_A && pressed)
		{
			RoxLogger::log() << "A pressed\n";
			m_move_x += 0.1f;
		}

		if (key == ::RoxInput::KEY_Q && pressed)
		{
			RoxLogger::log() << "Q pressed\n";

			m_move_y += 0.1f;
		}

		if (key == ::RoxInput::KEY_E && pressed)
		{
			RoxLogger::log() << "E pressed\n";
			m_move_y -= 0.1f;
		}


		
	}

	void onMouseScroll(int dx, int dy) override
	{
		m_rotate_x += dy * 0.1f;

		m_move_y += dy * 0.1f;

		RoxLogger::log() << "mouse scroll dx: " << dx << " dy: " << dy << "\n";
	}

	void onMouseMove(int x, int y) override
	{
		//RoxLogger::log() << "mouse move X: " << x << " Y: " << y << "\n";
	}

	void onMouseButton(::RoxInput::MOUSE_BOTTON button, bool pressed) override
	{
		//RoxLogger::log() << "mouse button " << button << " | " << pressed << "\n";
		if (button == ::RoxInput::MOUSE_BOTTON::MOUSE_LEFT && pressed)
		{
			
			
		}

		if (button == ::RoxInput::MOUSE_BOTTON::MOUSE_RIGHT && pressed)
		{
			RoxLogger::log() << "Right mouse button pressed\n";
		}

		if (button == ::RoxInput::MOUSE_BOTTON::MOUSE_MIDDLE && pressed)
		{
			RoxLogger::log() << "Middle mouse button pressed\n";
		}
	}

public:
	testCube() : m_rot(0.0f) {}

private:
	// Camera Movment and Rotation keys
	float m_move_z = 2;
	float m_move_x = 0;
	float m_move_y = 0;
	float m_rotate_z = 0;
	float m_rotate_x = 0;
	float m_rotate_y = 0;


	RoxScene::camera m_camera;
	RoxScene::mesh m_mesh;


	RoxRender::RoxVbo m_vbo;
	RoxRender::RoxShader m_shader;
	float m_rot;
};


int main(int argc, char** argv)
{
	testCube app;
	RoxLogger::log() << "App Created\n";
	//app.setTitle("Loading, please wait...");
	RoxLogger::log() << "Title " << app.getTitle() << "\n";
	
	app.startWindowed(100, 100, 640, 480, 0);
	//RoxLogger::log() << "exit success\n";

	return 0;
}