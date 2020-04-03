// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

#include <UEngine/Engine.h>
#include <URTR/DeferredRenderer.h>
#include <UScene/tool/SceneReflectionInit.h>
#include <UScene/tool/serialize/SerializerJSON.h>
#include <UScene/core/core>

#include <stdio.h>

#include <string>

using namespace Ubpa;
using namespace std;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    SceneReflectionInit();
    Scene scene("scene");

    auto [sobj0, tsfm0, camera] = scene.CreateSObj<Cmpt::Transform, Cmpt::Camera>("sobj0");
    auto [sobj1, tsfm1, geo1, mat1] = scene.CreateSObj<Cmpt::Transform, Cmpt::Geometry, Cmpt::Material>("sobj1");
    auto [sobj2, tsfm2, geo2, mat2] = scene.CreateSObj<Cmpt::Transform, Cmpt::Geometry, Cmpt::Material>("sobj2");
    auto [sobj3, tsfm3, geo3, mat3] = scene.CreateSObj<Cmpt::Transform, Cmpt::Geometry, Cmpt::Material>("sobj3");
    auto [sobj4, tsfm4, light4] = scene.CreateSObj<Cmpt::Transform, Cmpt::Light>("sobj4");

    geo1->SetPrimitive(new Sphere);
    geo2->SetPrimitive(new Square);
    geo3->SetPrimitive(new TriMesh(TriMesh::Type::Cube));
    auto brdf1 = new stdBRDF;
    auto brdf2 = new stdBRDF;
    auto brdf3 = new stdBRDF;
    string albedo_path = "../data/textures/rusted_iron/albedo.png";
    string roughness_path = "../data/textures/rusted_iron/roughness.png";
    string metalness_path = "../data/textures/rusted_iron/metallic.png";
    string normal_path = "../data/textures/rusted_iron/normal.png";
    brdf1->albedo_texture = ResourceMngr<Image>::Instance().GetOrCreate(albedo_path, albedo_path);
    brdf1->roughness_texture = ResourceMngr<Image>::Instance().GetOrCreate(roughness_path, roughness_path);
    brdf1->metalness_texture = ResourceMngr<Image>::Instance().GetOrCreate(metalness_path, metalness_path);
    brdf1->normal_map = ResourceMngr<Image>::Instance().GetOrCreate(normal_path, normal_path);
    brdf2->albedo_texture = ResourceMngr<Image>::Instance().GetOrCreate(albedo_path, albedo_path);
    brdf2->roughness_texture = ResourceMngr<Image>::Instance().GetOrCreate(roughness_path, roughness_path);
    brdf2->metalness_texture = ResourceMngr<Image>::Instance().GetOrCreate(metalness_path, metalness_path);
    brdf2->normal_map = ResourceMngr<Image>::Instance().GetOrCreate(normal_path, normal_path);
    brdf3->albedo_texture = ResourceMngr<Image>::Instance().GetOrCreate(albedo_path, albedo_path);
    brdf3->roughness_texture = ResourceMngr<Image>::Instance().GetOrCreate(roughness_path, roughness_path);
    brdf3->metalness_texture = ResourceMngr<Image>::Instance().GetOrCreate(metalness_path, metalness_path);
    brdf3->normal_map = ResourceMngr<Image>::Instance().GetOrCreate(normal_path, normal_path);
    mat1->SetMaterial(brdf1);
    mat2->SetMaterial(brdf2);
    mat3->SetMaterial(brdf3);

    tsfm0->SetPosition({ 0,0,8 });
    tsfm1->SetPosition({ -4,0,0 });
    tsfm1->SetScale({ 2.f });
    tsfm2->SetRotation({ vecf3{1,0,0}, to_radian(45.f) });
    tsfm3->SetPosition({ 4,0,0 });
    tsfm3->SetScale({ 1,2,1 });
    tsfm3->SetRotation({ vecf3{1,2,1}.normalize(), to_radian(45.f) });
    camera->Init(to_radian(60.f), 1280 / static_cast<float>(720));

    light4->light = new PointLight{ 100.f,{0.9f,0.9f,1.f} };
    tsfm4->SetPosition({ 0,4,0 });
    
    // ========================================================================================

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ===========================================================================================

    DeferredRenderer rtr;

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        sobj0->Get<Cmpt::Camera>()->SetAR(display_w / static_cast<float>(display_h));
        rtr.Render(&scene, sobj0, display_w, display_h);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
