#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "camera.h"
#include "Model.h"
#include "Actions.h"
#include "Verticles.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void processInputPianoKeys(GLFWwindow *window, float deltaTime);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//Actions
Actions actions(1.0f);
Verticles ver(true);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// build and compile shaders
	// -------------------------
	Shader lightingShader("shader.vs", "shader.fs");
	Shader lampShader("model.vs", "model.fs");
	Shader skyboxShader("cubeMap.vs", "cubeMap.fs");

	/*vector<std::string> faces
	{
		"textures/skymap/right.jpg",
		"textures/skymap/left.jpg",
		"textures/skymap/top.jpg",
		"textures/skymap/bottom.jpg",
		"textures/skymap/front.jpg",
		"textures/skymap/back.jpg"
	};*/

	vector<std::string> faces
	{
		"textures/skymap/purplenebulart.jpg",
		"textures/skymap/purplenebulalf.jpg",
		"textures/skymap/purplenebulaup.jpg",
		"textures/skymap/purplenebuladn.jpg",
		"textures/skymap/purplenebulaft.jpg",
		"textures/skymap/purplenebulabk.jpg"
	};


	unsigned int cubemapTexture = loadCubemap(faces);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(999.7f,  999.2f,  999.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture(((string)("textures/container2.png")).c_str());
	unsigned int specularMap = loadTexture(((string)("textures/container2_specular.png")).c_str());


	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	Model piano(((string)"obj/Piano2/Pianotex.obj"));
	Model key_white(((string)"obj/Piano2/white.obj"));
	Model key_black(((string)"obj/Piano2/black.obj"));
	Model paper(((string)"obj/Piano2/paper.obj"));
	Model piano_flap(((string)"obj/Piano2/flap.obj"));
	Model stick(((string)"obj/Piano2/stick.obj"));

	Model stage(((string)"obj/stage/stage2.obj"));
	Model lamp(((string)"obj/stage/lamp.obj"));

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		/*
		Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		lightingShader.setVec3("dirLight.direction", -2.3f, -3.0f, 5.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		// spotLight
		lightingShader.setVec3("spotLight[0].position", camera.Position);
		lightingShader.setVec3("spotLight[0].direction", camera.Front);
		lightingShader.setVec3("spotLight[0].ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("spotLight[0].diffuse", 0.7f, 0.7f, 0.7f);
		lightingShader.setVec3("spotLight[0].specular", 0.2f, 0.2f, 0.2f);
		lightingShader.setFloat("spotLight[0].constant", 1.0f);
		lightingShader.setFloat("spotLight[0].linear", 0.09);
		lightingShader.setFloat("spotLight[0].quadratic", 0.032);
		lightingShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// render the cube
		// glBindVertexArray(cubeVAO);
		// glDrawArrays(GL_TRIANGLES, 0, 36);*/

		// render containers
		//glBindVertexArray(cubeVAO);
		//for (unsigned int i = 0; i < 10; i++)
		//{
			// calculate the model matrix for each object and pass it to shader before drawing
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, cubePositions[i]);
		//	float angle = 20.0f * i;
		//	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		//	lightingShader.setMat4("model", model);

		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}


		//DRAW PIANO
		glm::mat4 base_pos = glm::mat4(1.0f);
		base_pos = glm::translate(base_pos, glm::vec3(0.0f, -1.2f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(base_pos, glm::vec3(0.8f, 0.8f, 0.8f));	// it's a bit too big for our scene, so scale it down
		lightingShader.setMat4("model", model);
		lightingShader.setFloat("material.shininess", 128.0f);
		piano.Draw(lightingShader);
		//DRAW KEYS
		glm::mat4 keys_pos = base_pos;
		keys_pos = glm::translate(keys_pos, glm::vec3(-0.72f, 0.66f, 0.75f));
		//whites
		glm::mat4 key_scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		for (unsigned int i = 0; i < 36; i++) {
			glm::mat4 key = keys_pos;
			key = glm::translate(key, glm::vec3(0.042f * i, 0.0f, 0.0f));
			key = glm::rotate(key, glm::radians(actions.get_piano_key_angle(i, true)), glm::vec3(1.0f, 0.0f, 0.0f));
			key = key_scale * key;
			lightingShader.setMat4("model", key);
			key_white.Draw(lightingShader);
		}
		//black
		key_scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 keys_pos_black = glm::translate(keys_pos, glm::vec3(0.02f, 0.0f, 0.0f));
		bool add_four = true;
		unsigned int when_blank = 2;
		unsigned int black_key_number = 0;
		for (unsigned int i = 0; i < 35; i++) {
			if (i == when_blank) {
				if (add_four) when_blank += 4;
				else when_blank += 3;
				add_four = !add_four;
				continue;
			}
			glm::mat4 key = keys_pos_black;
			key = glm::translate(key, glm::vec3(0.042f * i, 0.0f, 0.0f));
			key = glm::rotate(key, glm::radians(actions.get_piano_key_angle(black_key_number, false)), glm::vec3(1.0f, 0.0f, 0.0f));
			key = key_scale * key;
			lightingShader.setMat4("model", key);
			key_black.Draw(lightingShader);
			black_key_number++;
		}
		//PAPER
		model = base_pos;
		model = glm::translate(model, glm::vec3(0.00f, 1.02f, 0.64f));
		model = glm::rotate(model, glm::radians(81.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.18f, 0.01f, 0.16f));
		lightingShader.setMat4("model", model);
		paper.Draw(lightingShader);
		//FLAP
		model = base_pos;
		model = glm::translate(model, glm::vec3(-0.786f, 0.91f, -0.928f));
		model = glm::rotate(model, glm::radians(actions.get_flop_angle()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.86f, 0.825f, 0.870f));
		lightingShader.setMat4("model", model);
		piano_flap.Draw(lightingShader);
		//STICK
		model = base_pos;
		model = glm::translate(model, glm::vec3(0.77f, 0.89f, 0.52f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(actions.get_stick_angle()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.85f, 0.7f, 0.7f));
		lightingShader.setMat4("model", model);
		stick.Draw(lightingShader);
		
		//STAGE
		model = glm::translate(base_pos, glm::vec3(0.0f, -1.476f, 0.0f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(actions.get_stick_angle()), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.85f, 0.7f, 0.7f));
		lightingShader.setMat4("model", model);
		stage.Draw(lightingShader);
		//LAMP
		glDisable(GL_CULL_FACE);
		for (int i = 0; i < 3; i++) {
			model = glm::translate(base_pos, glm::vec3(-8.0f + 6.5f*i, 9.6f, 5.47f));
			lightingShader.setMat4("model", model);
			lamp.Draw(lightingShader);

			string name = "spotLight[";
			name.append(std::to_string(i+1));
			name.append("].");
			glm::vec3 position = glm::vec3(model[3][0], model[3][1], model[3][2]);
			//std::cout << position.x << " " << position.y << " " << position.z << "\t " << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z << std::endl;
			glm::vec3 direction = glm::vec3(base_pos[3][0] - model[3][0], base_pos[3][1] - model[3][1], base_pos[3][2] - model[3][2]);
			//glm::vec3 direction = glm::vec3(model[3][0] - base_pos[3][0] , model[3][1] - base_pos[3][1], model[3][2] - base_pos[3][2]);
			direction = glm::normalize(direction);
			//std::cout <<i<< " "<< direction.x << " " << direction.y << " " << direction.z << "\t " << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << std::endl;
			lightingShader.setVec3(name + "position", position);
			lightingShader.setVec3(name + "direction", direction);
			lightingShader.setVec3(name + "ambient", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3(name + "diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3(name + "specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat(name + "constant", 1.0f);
			lightingShader.setFloat(name + "linear", 0.09);
			lightingShader.setFloat(name + "quadratic", 0.032);
			lightingShader.setFloat(name + "cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat(name + "outerCutOff", glm::cos(glm::radians(15.0f)));
		}
		glEnable(GL_CULL_FACE);


		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightVAO);
		/*for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lampShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}*/


		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &skyboxVBO);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(SPACE, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LCTRL, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_OPEN_FLOP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_CLOSE_FLOP, deltaTime);
	}

	processInputPianoKeys(window, deltaTime);
}

void processInputPianoKeys(GLFWwindow *window, float deltaTime) {
	//WHITE
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 0, true);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 0, true);
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 1, true);
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 1, true);
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 2, true);
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 2, true);
	}

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 3, true);
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 3, true);
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 4, true);
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 4, true);
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 5, true);
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 5, true);
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 6, true);
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 6, true);
	}

	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 7, true);
	}
	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 7, true);
	}

	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 8, true);
	}
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 8, true);
	}

	if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 9, true);
	}
	if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 9, true);
	}



	//BLACK
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 0, false);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 0, false);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 1, false);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 1, false);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 2, false);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 2, false);
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 3, false);
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 3, false);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 4, false);
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 4, false);
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 5, false);
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 5, false);
	}
	if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
		actions.ProcessKeyboard(PIANO_PUSH_KEY, deltaTime, 6, false);
	}
	if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_RELEASE) {
		actions.ProcessKeyboard(PIANO_RELEASE_KEY, deltaTime, 6, false);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

