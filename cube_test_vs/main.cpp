#include "RoxApp/RoxApp.h"
#include "RoxRender/RoxVbo.h"
#include "RoxRender/RoxShader.h"
#include "RoxRender/RoxRender.h"
#include "RoxLogger/RoxLogger.h"

#include <sstream>

class testCube : public RoxApp::RoxApp
{
private:
	bool onSplash()
	{
		RoxLogger::log() << "Splash\n";

		RoxRender::setClearColor(1.0f, 0, 0, 1.0f);
		RoxRender::clear(true, true);

		return true;
	}

	void onInit()
	{
		RoxLogger::log() << "Init\n";

		RoxRender::setClearColor(0.8f, 0, 0, 0.0f);
		RoxRender::setClearDepth(1.0f);
		RoxRender::DepthTest::enable(RoxRender::DepthTest::LESS);

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

		/*
		if(RoxRender::get_render_api()==RoxRender::render_api_directx11)
		{
			static RoxSystem::compiled_shaders_provider csp;
			csp.set_load_path( RoxSystem::get_app_path() );
			csp.set_save_path( RoxSystem::get_app_path() );
			RoxRender::set_compiled_shaders_provider( &csp );
		}
		*/

		const char* vs_code =
			"varying vec4 color;"
			"void main()"
			"{"
			"color=gl_Color;"
			"gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;"
			"}";

		const char* ps_code =
			"varying vec4 color;"
			"void main()"
			"{"
			"gl_FragColor=color;"
			"}";

		m_shader.addProgram(RoxRender::RoxShader::VERTEX, vs_code);
		m_shader.addProgram(RoxRender::RoxShader::PIXEL, ps_code);
	}

	void onFrame(unsigned int dt)
	{
		//RoxLogger::log() << "Frame\n";
		RoxRender::clear(true, true);

		m_rot += dt * 0.05f;
		if (m_rot > 360)
			m_rot = 0;

		RoxMath::Matrix4 mv;
		mv.translate(0, 0, -2.0f);
		mv.rotate(30.0f, 1.0f, 0.0f, 0.0f);
		mv.rotate(m_rot, 0.0f, 1.0f, 0.0f);
		RoxRender::setModelviewMatrix(mv);

		m_shader.bind();
		m_vbo.bind();
		m_vbo.draw();
		m_vbo.unbind();
		m_shader.unbind();

		static unsigned int fps_counter = 0, fps_update_timer = 0;
		++fps_counter;
		fps_update_timer += dt;
		if (fps_update_timer > 1000)
		{
			std::ostringstream os;
			os << "test cube " << fps_counter << " fps";
			std::string str = os.str();
			setTitle(str.c_str());

			fps_update_timer %= 1000;
			fps_counter = 0;
		}
	}

	void onResize(unsigned int w, unsigned int h)
	{
		RoxLogger::log() << "on_resize " << w << " " << h << "\n";

		if (!w || !h)
			return;

		RoxMath::Matrix4 proj;
		proj.perspective(70.0f, float(w) / h, 0.01f, 100.0f);
		RoxRender::setProjectionMatrix(proj);
	}

	void onFree()
	{
		RoxLogger::log() << "on_free\n";

		m_vbo.release();
		m_shader.release();
	}

	//void on_keyboard(unsigned int key, bool pressed)
	//{
	//	/*if ((key == RoxInput::KEY_BACK || key == RoxInput::KEY_ESCAPE) && !pressed)
	//		finish();*/
	//}

public:
	testCube() : m_rot(0.0f) {}

private:
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
	RoxLogger::log() << "exit success\n";

	return 0;
}