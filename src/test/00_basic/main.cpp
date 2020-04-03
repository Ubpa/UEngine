// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

#include <UEngine/Engine.h>
#include <UEngine/SceneMngr.h>

#include <stdio.h>

#include <string>

using namespace Ubpa;
using namespace std;

int main(int, char**)
{
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

    SceneMngr::Instance().Active(&scene, sobj0);

    Engine::Instance().Init("Ubpa@2020 UEngine - 00 basic");

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    Engine::Instance().Loop([&]() {
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
    });

    // Cleanup
    Engine::Instance().CleanUp();

    return 0;
}
