// ImGui SFML binding with OpenGL
// https://github.com/Mischa-Alff/imgui-backends

#ifndef IMGUI_SFML_RENDERING_BACKEND
#define IMGUI_SFML_RENDERING_BACKEND
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <iostream>

namespace ImGui
{
    namespace ImImpl
    {
        static sf::RenderTarget* ImImpl_rtarget;
        static sf::Texture ImImpl_fontTex;

        static void ImImpl_RenderDrawLists(ImDrawData *draw_data)
        {
            sf::RenderStates states;
            states.blendMode = sf::BlendMode(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha);

            for (int n = 0; n < draw_data->CmdListsCount; n++)
            {
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                const unsigned char* vtx_buffer_ = (const unsigned char*)(&(cmd_list->VtxBuffer.front()));
                const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
                sf::Vertex* vtx_buffer = (sf::Vertex*)vtx_buffer_;

                for (int i = 0; i < cmd_list->VtxBuffer.size(); i++)
                {
                    vtx_buffer[i].texCoords.x*=ImImpl::ImImpl_fontTex.getSize().x;
                    vtx_buffer[i].texCoords.y*=ImImpl::ImImpl_fontTex.getSize().y;
                }

                for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
                {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                    //printf("size: %d count: %d\n", cmd_i, pcmd->ElemCount);
                    if (pcmd->UserCallback)
                    {
                        pcmd->UserCallback(cmd_list, pcmd);
                    }
                    else
                    {
                        states.texture = (sf::Texture*)pcmd->TextureId;
                        if(!states.texture) std::cout<<"Invalid texture!"<<std::endl;
                        // Populate the vertex array with indexed vertices since SFML does not allow indexed drawing
                        sf::VertexArray m_vertices(sf::Triangles, pcmd->ElemCount);
                        for (size_t i = 0; i < pcmd->ElemCount; i++)
                        {
                            m_vertices[i] = vtx_buffer[idx_buffer[i]];
                        }
                        ImImpl::ImImpl_rtarget->draw(m_vertices, states);
                        //ImImpl::ImImpl_rtarget->draw(&vtx_buffer[idx_buffer], pcmd->ElemCount, sf::Triangles, states);
                    }

                    idx_buffer += pcmd->ElemCount;
                }
            }
        }
    }
    namespace SFML
    {
        static void SetRenderTarget(sf::RenderTarget &target) { ImImpl::ImImpl_rtarget = &target; }
        static void InitImGuiRendering()
        {
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2(ImImpl::ImImpl_rtarget->getSize().x, ImImpl::ImImpl_rtarget->getSize().y);
            io.RenderDrawListsFn = ImImpl::ImImpl_RenderDrawLists;
            unsigned char* pixels;
            int width, height;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            ImImpl::ImImpl_fontTex.create(width, height);
            ImImpl::ImImpl_fontTex.update(pixels);
            io.Fonts->TexID = (void*)&ImImpl::ImImpl_fontTex;
            io.Fonts->ClearInputData();
            io.Fonts->ClearTexData();
        }
        static void UpdateImGuiRendering()
        {
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2(ImImpl::ImImpl_rtarget->getSize().x, ImImpl::ImImpl_rtarget->getSize().y);
        }
    }
}
#endif
