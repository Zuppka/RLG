/*

[10] Star types: http://www.enchantedlearning.com/subjects/astronomy/stars/startypes.shtml
*/


#include <ImGui/imgui.h>
#include <ImGui/imgui-events-SFML.h>
#include <ImGui/imgui-rendering-SFML.h>

#include <SFML/Graphics.hpp>

#include <modules/worldman.h>

#include <string.h>

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

void ShowOptionsWindow(bool* opened) {

    if (ImGui::Begin("Options", opened, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Graphics options
        if (ImGui::TreeNode("Graphics")) {
            ImGui::TreePop();
        }
        // Audio and sound options
        if (ImGui::TreeNode("Audio")) {
            ImGui::TreePop();
        }
        // Menu style options
        if (ImGui::TreeNode("Style")) {
            ImGui::ShowStyleEditor();
            ImGui::TreePop();
        }
        // Font style options
        if (ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size)) {
            ImGui::TextWrapped("Tip: Load fonts with io.Fonts->AddFontFromFileTTF().");
            ImFontAtlas* atlas = ImGui::GetIO().Fonts;

            if (ImGui::TreeNode("Atlas texture")) {
                ImGui::Text("%dx%d pixels", atlas->TexWidth, atlas->TexHeight);
                ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
                ImGui::TreePop();
            }
            ImGui::PushItemWidth(100);
            for (int i = 0; i < atlas->Fonts.Size; i++) {
                ImFont* font = atlas->Fonts[i];
                ImGui::BulletText("Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
                ImGui::TreePush((void*)i);
                if (i > 0) { ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) { atlas->Fonts[i] = atlas->Fonts[0]; atlas->Fonts[0] = font; } }
                ImGui::PushFont(font);
                ImGui::Text("The quick brown fox jumps over the lazy dog");
                ImGui::PopFont();

                if (ImGui::TreeNode("Details")) {
                    ImGui::DragFloat("font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");             // scale only this font
                    ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                    ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
                        ImGui::BulletText("Input %d: \'%s\'", config_i, font->ConfigData[config_i].Name);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            static float window_scale = 1.0f;
            ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
            ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
            ImGui::PopItemWidth();
            ImGui::SetWindowFontScale(window_scale);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

void ShowLoginWindow(bool* opened) {

    if (ImGui::Begin("Login", opened, ImGuiWindowFlags_AlwaysAutoResize)) {

    }
}

// WorldMan Window
void WorldManWindow(bool* opened) {

    static float bg_alpha = 0.65f;
    static bool show_help = false;
    static bool show_wm_edit = false;
    static bool get_id = false;
    Worldman ent;
    static bool magField = true;
    static int id = 0, parID = 0, type = 0, numSats = 0, coreType = 0, temp = 5770, starType = 0;
    static float mass = 1.0, radius = 1.0, dist = 1.0;
    static char name[32] = "Pandora";
    static std::string msg;
    ent.init();

    //
    if (show_help) {
        ImGui::Begin("Worldman Help", &show_help, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("This is used to generate entities.\n");
        ImGui::Separator();
        ImGui::Text("Select an option from 'Random' for completely random generated entity\n");
        ImGui::Text("or pick 'Manual' to enter your own parameters.\n");
        ImGui::End();
    }

    if (get_id) {
        ImGui::Begin("Generate Random", &get_id, ImGuiWindowFlags_AlwaysAutoResize);
        // Select entity type and parent ID
        ImGui::Combo("Entity", &type, "Star System\0Star\0Planet\0Moon\0Minor\0\0");
        ImGui::SameLine();
        ImGui::DragInt("Parent ID", &parID, 1, 0, 999);
        static bool check = false;
        ImGui::SameLine();
        ImGui::Checkbox("Manual", &check);

        // Display feedback messages here
        ImGui::Text("%s\n", msg.c_str());
        // Pressing OK passes ID value on to entity generating function
        if (ImGui::Button("Generate", ImVec2(120,0))) {
            // Generate Star System
            if (!check && type == 0) {
                ent.generateStarSystem();
                msg = "Star System generated!";
            }
            // Generate Entity
            else {
                if (check) {
                    // Update with manual data info
                }
                id = ent.generateEntity(type, parID);
                if (id) {
                    ent.loadEntity(&ent, id);
                    msg = ent.name;
                    msg += " generated!";
                }
                else
                    msg = "Invalid ID!";
            }
        }
        ImGui::SameLine();
        // Otherwise if Cancel is pressed, close the window
        if (ImGui::Button("Close", ImVec2(120,0))) { get_id = false; }
        ImGui::Separator();

        // Additional options for manual entity generation
        if (check) {
            if (type == 1) {
                ImGui::PushItemWidth(100);
                ImGui::DragFloat("Solar Mass", &mass, 0.1, 0.1, 100); ImGui::SameLine(250);
                ImGui::DragFloat("Solar Radius", &radius, 0.1, 0.1, 20);
                ImGui::DragInt("Temperature", &temp, 100, 3000, 25000); ImGui::SameLine(250);
                ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
                ImGui::InputInt("Planets", &numSats); ImGui::SameLine(250);
                ImGui::Combo("Star Type", &coreType, "Type M\0Type K\0Type G\0Type F\0Type A\0Type B\0Type O\0");
                ImGui::PopItemWidth();
            }
            if (type == 2) {
                ImGui::PushItemWidth(100);
                ImGui::DragFloat("Earth Mass", &mass, 0.1, 0.1, 300); ImGui::SameLine(250);
                ImGui::DragFloat("Earth Radius", &radius, 0.1, 0.1, 100);
                ImGui::DragFloat("Semi-Major Axis", &dist, 0.1, 0.1, 30); ImGui::SameLine(250);
                ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
                ImGui::InputInt("Moons", &numSats); ImGui::SameLine(250);
                ImGui::PushItemWidth(150);
                ImGui::Combo("Core Composition", &starType, "None\0Small Nickel-Iron\0Large Nickel-Iron\0Hydrous Silicate\0Metallic Hydrogen\0");
                ImGui::Checkbox("Magnetic Field", &magField);
                ImGui::PopItemWidth();
            }
            if (type == 3) {
                ImGui::DragFloat("Moon Mass", &mass, 1, 0, 10);
                ImGui::DragFloat("Moon Radius", &radius, 1, 0, 100);
                ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
            }
            if (type == 4) {
                ImGui::DragFloat("Minor Mass", &mass, 1, 0, 1);
                ImGui::DragFloat("Minor Radius", &radius, 1, 0, 100);
                ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
            }
            if (type == 5) {
                ImGui::DragFloat("Black hole Mass", &mass, 1, 0, 100);
                ImGui::DragFloat("Black hole Radius", &radius, 1, 0, 100);
                ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
            }
        }
        ImGui::End();
    }

    // Create the module's window here
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("WorldMan", opened, ImVec2(640,480), bg_alpha, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    //ImGui::PushItemWidth(-140);  // Right align, keep 140 pixels for labels

    // Window menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            ImGui::MenuItem("Generate Entity", NULL, &get_id);
            ImGui::MenuItem("Search", NULL, &show_help);
            ImGui::MenuItem("Edit", NULL, &show_wm_edit);
            ImGui::Separator();
            ImGui::MenuItem("Help", NULL, &show_help);
            if (ImGui::MenuItem("Exit")) { *opened = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (ImGui::CollapsingHeader("Stars")) {
        for (int i = 0; i < 5; i++) {
            if (ImGui::TreeNode((void*)i, "Star %d", i)) {
                ent.loadEntity(&ent, i);
                ent.printEntityInfo(i);

                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}

// MapMan Window
void MapManWindow(bool* opened) {
    static float bg_alpha = 0.65f;
    static bool show_help = false;
    //
    if (show_help) {
        ImGui::Begin("MapMan Help", &show_help, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("This is used to generate map tiles.\n");
        ImGui::Separator();
        ImGui::Text("???\n");
        ImGui::Text("\n");
        ImGui::End();
    }

    // Create the module's window here
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("MapMan", opened, ImVec2(640,480), bg_alpha, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Window menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            ImGui::MenuItem("Help", NULL, &show_help);
            if (ImGui::MenuItem("Exit")) { *opened = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

// RezMan Window
void RezManWindow(bool* opened) {
    static float bg_alpha = 0.65f;
    static bool show_help = false;
    //
    if (show_help) {
        ImGui::Begin("RezMan Help", &show_help, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("This is used to generate resources.\n");
        ImGui::Separator();
        ImGui::Text("???\n");
        ImGui::Text("\n");
        ImGui::End();
    }

    // Create the module's window here
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("RezMan", opened, ImVec2(640,480), bg_alpha, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Window menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            ImGui::MenuItem("Help", NULL, &show_help);
            if (ImGui::MenuItem("Exit")) { *opened = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

// NetMan Window
void NetManWindow(bool* opened) {
    static float bg_alpha = 0.65f;
    static bool show_help = false;
    //
    if (show_help) {
        ImGui::Begin("NetMan Help", &show_help, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("This is used to create network sockets between client and server.\n");
        ImGui::Separator();
        ImGui::Text("???\n");
        ImGui::Text("\n");
        ImGui::End();
    }

    // Create the module's window here
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("NetMan", opened, ImVec2(640,480), bg_alpha, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Window menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            ImGui::MenuItem("Help", NULL, &show_help);
            if (ImGui::MenuItem("Exit")) { *opened = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

// Main menu bar
static void ShowMenuBar() {
    // Examples apps
    static bool show_options = false;
    static bool show_login = false;
    static bool show_worldman = false;
    static bool show_mapman = false;
    static bool show_rezman = false;
    static bool show_netman = false;
    static bool show_app_metrics = false;
    static bool show_app_about = false;

    if (show_options) ShowOptionsWindow(&show_options);
    if (show_login) ShowLoginWindow(&show_login);
    if (show_worldman) WorldManWindow(&show_worldman);
    if (show_mapman) MapManWindow(&show_mapman);
    if (show_rezman) RezManWindow(&show_rezman);
    if (show_netman) NetManWindow(&show_netman);
    if (show_app_metrics) ImGui::ShowMetricsWindow(&show_app_metrics);
    if (show_app_about) {
        ImGui::Begin("About ImGui", &show_app_about, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("ImGui %s", ImGui::GetVersion());
        ImGui::Separator();
        ImGui::Text("By Omar Cornut and all github contributors.");
        ImGui::Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::End();
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Login")) {}
            ImGui::Separator();
            ImGui::MenuItem("Options", NULL, &show_options);
            if (ImGui::MenuItem("Quit", "Alt+F4")) { }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Data Managers")) {
            ImGui::MenuItem("WorldMan", NULL, &show_worldman);
            ImGui::MenuItem("MapMan", NULL, &show_mapman);
            ImGui::MenuItem("RezMan", NULL, &show_rezman);
            ImGui::MenuItem("NetMan", NULL, &show_netman);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
            ImGui::MenuItem("About ImGui", NULL, &show_app_about);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// Modify the ImGui visual style
void UpdateStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    // Interface shape
    style.FramePadding      = ImVec2(4, 4);  // Padding within a framed rectangle (used by most widgets)
    style.FrameRounding     = 16.0f;        // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    style.ScrollbarSize     = 20.0f;        // Width of the vertical scrollbar
    style.ScrollbarRounding = 16.0f;        // Radius of grab corners rounding for scrollbar
    style.GrabMinSize       = 20.0f;        // Minimum width/height of a grab box for slider/scrollbar
    style.GrabRounding      = 16.0f;        // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.

    // Master colour control to allow uniform colour style
    float r = 1.00f, g = 0.60f, b = 0.20f;
    // Interface colour
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
    style.Colors[ImGuiCol_Border]               = ImVec4(r, g, b, 0.60f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(r, g, b, 0.40f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(r, g, b, 0.20f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(r, g, b, 0.60f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(r, g, b, 0.10f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(r, g, b, 0.30f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(r, g, b, 0.40f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(r, g, b, 0.60f);
    style.Colors[ImGuiCol_Button]               = ImVec4(r, g, b, 0.60f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(r, g, b, 0.80f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(r, g, b, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(r, g, b, 0.20f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(r, g, b, 0.40f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(r, g, b, 0.40f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(r, g, b, 0.40f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(r, g, b, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(r, g, b, 0.80f);
    style.Colors[ImGuiCol_CloseButton]          = ImVec4(r, g, b, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered]   = ImVec4(r, g, b, 0.60f);
    style.Colors[ImGuiCol_CloseButtonActive]    = ImVec4(r, g, b, 1.00f);
}

int main() {
    sf::RenderWindow win{{SCREEN_WIDTH, SCREEN_HEIGHT}, "window"};
    //win.setFramerateLimit(60);

    ImGui::SFML::SetWindow(win);
    ImGui::SFML::SetRenderTarget(win);
    ImGui::SFML::InitImGuiRendering();
    ImGui::SFML::InitImGuiEvents();

    sf::Color clearColor(50, 50, 50);   // Background colour
    UpdateStyle();

    // Main loop
    while(win.isOpen()) {
        ImGui::SFML::UpdateImGui();
        ImGui::SFML::UpdateImGuiRendering();
        sf::Event e;
        // Send events to SFML event backend
        while(win.pollEvent(e)) {
            ImGui::SFML::ProcessEvent(e);
            if(e.type == sf::Event::Closed) {
                win.close();
            }
        }
        // Display menu bar at the top of the window
        ShowMenuBar();

        // Clear screen by redrawing background colour and redraw GUI
        win.clear(clearColor);
        ImGui::Render();
        win.display();
    }

    ImGui::Shutdown();

    return 0;
}
