#include "window.h"
#include "shader.h"
#include "camera.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "frame_buffer.h"
#include "texture.h"


namespace Hub
{
	int windowWidth = 800;
	int windowHeight = 600;
	float deltaTime = 0.0f; // 存储上一帧渲染所用时间（当前时间戳-上一帧时间戳）
	float lastFrame = 0.0f; // 上一帧时间戳

	bool beforeMode = true;
	bool shadowKeyPressed = false;
	bool shadows = false;

	Camera camera;

	// 实现键盘移动摄像机
	void processInput(GLFWwindow* window)
	{
		const float cameraSpeed = 5.0f * deltaTime; // 保证摄像机移动速度和硬件无关

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.processKeyBoard(CameraMovement::FORWORD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.processKeyBoard(CameraMovement::BACKWORD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.processKeyBoard(CameraMovement::LEFT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.processKeyBoard(CameraMovement::RIGHT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !shadowKeyPressed)
		{
			shadows = !shadows;
			shadowKeyPressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
		{
			shadowKeyPressed = false;
		}
	}

	// 鼠标控制摄像机
	bool firstMouse = true;
	glm::vec2 lastPos(windowWidth / 2.0f, windowHeight / 2.0f); // window center

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS))
		{
			return;
		}
		glm::vec2 pos(xpos, ypos);
		if (firstMouse)
		{
			lastPos = pos;
			firstMouse = false;
		}
		GLfloat xoffset = pos.x - lastPos.x;
		GLfloat yoffset = lastPos.y - pos.y; // 由于这里y坐标范围从下往上
		lastPos = pos;
		camera.processMouseMovement(-xoffset, -yoffset);
	}

	// 滚轮调整摄像机fov, 实现缩放效果
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.processMouseScroll(static_cast<float>(yoffset));
	}

	SPVertexArray VAO;
	void generatePlaneVAO()
	{
		static float planeVertices[] = {
			// positions            // normals         // texcoords
			 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
			-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

			 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
			-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
			 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
		};

		VAO = VertexArray::create();
		auto VBO = VertexBuffer::create(planeVertices, sizeof(planeVertices), BufferUsage::StaticDraw);

		VAO->bindAttribute(0, 3, *VBO, Type::Float, 8 * sizeof(float), 0);
		VAO->bindAttribute(1, 3, *VBO, Type::Float, 8 * sizeof(float), 3 * sizeof(float));
		VAO->bindAttribute(2, 2, *VBO, Type::Float, 8 * sizeof(float), 6 * sizeof(float));
		glBindVertexArray(0);
	}

	// renders a 1x1 3D cube in NDC
	
	SPVertexArray cubeVAO;
	void renderCube()
	{
		static float cubeVertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		if (!cubeVAO)
		{
			cubeVAO = VertexArray::create();
			auto cubeVBO = VertexBuffer::create(cubeVertices, sizeof(cubeVertices), BufferUsage::StaticDraw);
			cubeVAO->bindAttribute(0, 3, *cubeVBO, Type::Float, 8 * sizeof(float), 0);
			cubeVAO->bindAttribute(1, 3, *cubeVBO, Type::Float, 8 * sizeof(float), 3 * sizeof(float));
			cubeVAO->bindAttribute(2, 2, *cubeVBO, Type::Float, 8 * sizeof(float), 6 * sizeof(float));
			glBindVertexArray(0);
		}
		
		glBindVertexArray(*cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	void renderScene(Shader& shader, VertexArray& vao)
	{
		//// floor
		//glm::mat4 model = glm::mat4(1.0f);
		//shader.setMatirx4("model", model);
		//glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		// room
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(5.0f));
		shader.setMatirx4("model", model);
		glDisable(GL_CULL_FACE);
		shader.setInt("reverse_normal", 1);
		renderCube();
		shader.setInt("reverse_normal", 0);
		glEnable(GL_CULL_FACE);
		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMatirx4("model", model);
		renderCube();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.75f));
		shader.setMatirx4("model", model);
		renderCube();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMatirx4("model", model);
		renderCube();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
		model = glm::scale(model, glm::vec3(0.5f));
		shader.setMatirx4("model", model);
		renderCube();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.75f));
		shader.setMatirx4("model", model);
		renderCube();
	}

	SPVertexArray quadVAO;
	void renderQuad()
	{
		if (!quadVAO)
		{
			static float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			quadVAO = VertexArray::create();
			auto quadVBO = VertexBuffer::create(quadVertices, sizeof(quadVertices), BufferUsage::StaticDraw);
			quadVAO->bindAttribute(0, 3, *quadVBO, Type::Float, 5 * sizeof(float), 0);
			quadVAO->bindAttribute(1, 2, *quadVBO, Type::Float, 5 * sizeof(float), 3 * sizeof(float));
			glBindVertexArray(0);
		}
		glBindVertexArray(*quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void test()
	{
		Window hWindow(windowWidth, windowHeight);
		auto window = hWindow.getNativeHandle();
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		Shader shader("./shader/shader.vs", "./shader/shader.fs");
		Shader debugShader("./shader/debug.vs", "./shader/debug.fs");
		Shader depthShader("./shader/shadow_mapping_depth.vs", "./shader/shadow_mapping_depth.fs");

		generatePlaneVAO();

		const char* filePath = "../Asset/wood.png";
		auto floorTexture = Texture::create(filePath);
		floorTexture->setWrapping(Wrapping::axis_t::S, Wrapping::wrapping_t::Repeat);
		floorTexture->setWrapping(Wrapping::axis_t::T, Wrapping::wrapping_t::Repeat);
		floorTexture->setFilter(Filter::operator_t::Min, Filter::filter_t::LinearMipmapLinear);
		floorTexture->setFilter(Filter::operator_t::Mag, Filter::filter_t::Linear);

		// configure depth map fbo
		auto depthMapFBO = FrameBuffer::create();
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		// create depth texture
		auto depthMap = Texture::create();
		depthMap->image2D(nullptr, Format::DEPTH, SHADOW_WIDTH, SHADOW_HEIGHT, Type::Float);
		depthMap->setFilter(Filter::operator_t::Min, Filter::filter_t::Nearest);
		depthMap->setFilter(Filter::operator_t::Mag, Filter::filter_t::Nearest);
		depthMap->setWrapping(Wrapping::axis_t::S, Wrapping::wrapping_t::ClampBorder);
		depthMap->setWrapping(Wrapping::axis_t::T, Wrapping::wrapping_t::ClampBorder);
		Color borderColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		depthMap->setBorderColor(borderColor);
		// attach depth texture as fbo's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, *depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);

		float nearPlane = 1.0;
		float farPlane = 7.5;

		debugShader.use();
		debugShader.setInt("depthMap", 0);
		debugShader.setFloat("near", nearPlane);
		debugShader.setFloat("far", farPlane);

		shader.use();
		shader.setInt("floorTexture", 0);
		shader.setInt("depthMap", 1);

		glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
		float aspect = windowWidth / windowHeight * 1.0f;
		
		while (!hWindow.shouldClose())
		{
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			glfwPollEvents();
			processInput(window);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// render depth of scene to texture from light's perspective
			float nearPlane = 1.0f, farPlane = 7.5f;
			glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
			glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 lightSpaceMatrix = lightProjection * lightView;
			depthShader.use();
			depthShader.setMatirx4("lightSpaceMatrix", lightSpaceMatrix);
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, *depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *floorTexture);
			//glCullFace(GL_FRONT); // solve peter panning issue, but it not work perfectly fine on plane: plane will be removed.
			renderScene(depthShader, *VAO);
			//glCullFace(GL_BACK);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			glViewport(0, 0, windowWidth, windowHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			auto view = camera.getViewMatrix();
			auto projection = camera.getProjectionMatrix(aspect);
			shader.use();
			shader.setMatirx4("view", view);
			shader.setMatirx4("projection", projection);
			// set light uniform
			shader.setVec3("viewPos", camera.getPosition());
			shader.setVec3("lightPos", lightPos);
			shader.setMatirx4("lightSpaceMatrix", lightSpaceMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *floorTexture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, *depthMap);
			renderScene(shader, *VAO);

			//// render depth map to quad for visual debugging
			//debugShader.use();
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, *depthMap);
			//renderQuad();

			glBindVertexArray(0);
			glfwSwapBuffers(window);
		}
		glfwTerminate();
	}

	void test2()
	{
		Window hWindow(windowWidth, windowHeight);
		auto window = hWindow.getNativeHandle();
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		Shader shader("./shader/shader2.vs", "./shader/shader2.fs");
		Shader depthShader("./shader/cube_mapping_depth.vs", "./shader/cube_mapping_depth.fs", "./shader/cube_mapping_depth.gs");

		generatePlaneVAO();

		const char* filePath = "../Asset/wood.png";
		auto floorTexture = Texture::create(filePath);
		floorTexture->setWrapping(Wrapping::axis_t::S, Wrapping::wrapping_t::Repeat);
		floorTexture->setWrapping(Wrapping::axis_t::T, Wrapping::wrapping_t::Repeat);
		floorTexture->setFilter(Filter::operator_t::Min, Filter::filter_t::LinearMipmapLinear);
		floorTexture->setFilter(Filter::operator_t::Mag, Filter::filter_t::Linear);

		auto depthMapFBO = FrameBuffer::create();

		auto depthCubeMap = Texture::create(Hub::TextureCubeMap);
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		depthCubeMap->cubeMapImage2D(SHADOW_WIDTH, SHADOW_HEIGHT);
		depthCubeMap->setFilter(Filter::operator_t::Mag, Filter::filter_t::Nearest);
		depthCubeMap->setFilter(Filter::operator_t::Min, Filter::filter_t::Nearest);
		depthCubeMap->setWrapping(Wrapping::axis_t::R, Wrapping::wrapping_t::ClampEdge);
		depthCubeMap->setWrapping(Wrapping::axis_t::S, Wrapping::wrapping_t::ClampEdge);
		depthCubeMap->setWrapping(Wrapping::axis_t::T, Wrapping::wrapping_t::ClampEdge);

		glBindFramebuffer(GL_FRAMEBUFFER, *depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *depthCubeMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);

		shader.use();
		shader.setInt("diffuseTexture", 0);
		shader.setInt("depthMap", 1);

		glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
		float aspect = SHADOW_WIDTH / SHADOW_HEIGHT * 1.0f;

		// render loop
		while (!hWindow.shouldClose())
		{
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			glfwPollEvents();
			processInput(window);
			// move light position over time
			//lightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);;

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float near = 1.0f;
			float far = 25.0f;
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

			std::vector<glm::mat4> shadowTransforms;
			static std::vector<glm::vec3> vecs = {
				{1.0, 0.0, 0.0}, {0.0, -1.0, 0.0},
				{-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0},
				{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
				{0.0, -1.0, 0.0}, {0.0, 0.0, -1.0},
				{0.0, 0.0, 1.0}, {0.0, -1.0, 0.0},
				{0.0, 0.0, -1.0}, {0.0, -1.0, 0.0}
			};
			for (size_t i = 0; i < vecs.size(); i += 2)
			{
				auto& front = vecs[i];
				auto targetPos = lightPos + front;
				auto& up = vecs[i + 1];
				auto view = glm::lookAt(lightPos, targetPos, up);
				shadowTransforms.push_back(shadowProj * view);
			}

			// 1. first render to depth cubemap
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, *depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			// configure shader and matrix
			depthShader.use();
			for (uint i = 0; i < 6; ++i)
			{
				auto key = "shadowMatrices[" + std::to_string(i) + "]";
				depthShader.setMatirx4(key.c_str(), shadowTransforms[i]);
			}
			depthShader.setFloat("far_plane", far);
			depthShader.setVec3("lightPos", lightPos);
			renderScene(depthShader, *VAO);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 2. then render scene as normal with shadow mapping
			glViewport(0, 0, windowWidth, windowHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// configure shader and matrix
			auto view = camera.getViewMatrix();
			auto projection = camera.getProjectionMatrix(aspect);
			shader.use();
			shader.setMatirx4("view", view);
			shader.setMatirx4("projection", projection);
			// set light uniform
			shader.setVec3("viewPos", camera.getPosition());
			shader.setVec3("lightPos", lightPos);
			shader.setFloat("far_plane", far);
			shader.setInt("shadows", shadows);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *floorTexture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, *depthCubeMap);
			renderScene(shader, *VAO);

			glBindVertexArray(0);
			glfwSwapBuffers(window);
		}
		glfwTerminate();
		
	}

}


int main()
{
	Hub::test2();
	return 0;
}