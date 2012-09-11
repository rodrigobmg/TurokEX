// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION: Renderer System
//
//-----------------------------------------------------------------------------

#include "SDL_opengl.h"
#include "gl.h"
#include "kernel.h"
#include "render.h"
#include "common.h"
#include "mathlib.h"

CVAR_EXTERNAL(cl_fov);

//
// R_DrawTestCube
//

static void R_DrawTestCube(void)
{
    GL_SetState(GLSTATE_CULL, false);
    GL_BindTextureName("textures/default.tga");
    dglEnable(GL_DEPTH_TEST);

    dglColor4ub(255, 255, 255, 255);
    dglBegin(GL_POLYGON);
    dglTexCoord2f(0, 0);
    dglVertex3f(-32, 32, 32);
    dglTexCoord2f(1, 0);
    dglVertex3f(32, 32, 32);
    dglTexCoord2f(1, 1);
    dglVertex3f(32, -32, 32);
    dglTexCoord2f(0, 1);
    dglVertex3f(-32, -32, 32);
    dglEnd();
    dglBegin(GL_POLYGON);
    dglTexCoord2f(0, 0);
    dglVertex3f(32, 32, 32);
    dglTexCoord2f(1, 0);
    dglVertex3f(32, 32, -32);
    dglTexCoord2f(1, 1);
    dglVertex3f(32, -32, -32);
    dglTexCoord2f(0, 1);
    dglVertex3f(32, -32, 32);
    dglEnd();
    dglBegin(GL_POLYGON);
    dglTexCoord2f(0, 0);
    dglVertex3f(32, 32, -32);
    dglTexCoord2f(1, 0);
    dglVertex3f(-32, 32, -32);
    dglTexCoord2f(1, 1);
    dglVertex3f(-32, -32, -32);
    dglTexCoord2f(0, 1);
    dglVertex3f(32, -32, -32);
    dglEnd();
    dglBegin(GL_POLYGON);
    dglTexCoord2f(0, 0);
    dglVertex3f(-32, 32, -32);
    dglTexCoord2f(1, 0);
    dglVertex3f(-32, 32, 32);
    dglTexCoord2f(1, 1);
    dglVertex3f(-32, -32, 32);
    dglTexCoord2f(0, 1);
    dglVertex3f(-32, -32, -32);
    dglEnd();
    dglBegin(GL_POLYGON);
    dglTexCoord2f(0, 0);
    dglVertex3f(-32, 32, -32);
    dglTexCoord2f(1, 0);
    dglVertex3f(32, 32, -32);
    dglTexCoord2f(1, 1);
    dglVertex3f(32, 32, 32);
    dglTexCoord2f(0, 1);
    dglVertex3f(-32, 32, 32);
    dglEnd();
    dglBegin(GL_POLYGON);
    dglTexCoord2f(0, 0);
    dglVertex3f(-32, -32, 32);
    dglTexCoord2f(1, 0);
    dglVertex3f(32, -32, 32);
    dglTexCoord2f(1, 1);
    dglVertex3f(32, -32, -32);
    dglTexCoord2f(0, 1);
    dglVertex3f(-32, -32, -32);
    dglEnd();

    GL_SetState(GLSTATE_CULL, true);
    dglDisable(GL_DEPTH_TEST);
}

//
// R_DrawSection
//

void R_DrawSection(mdlsection_t *section)
{
    if(section->flags & MDF_NOCULLFACES)
    {
        GL_SetState(GLSTATE_CULL, false);
    }

    if(section->flags & MDF_MASKED)
    {
        GL_SetState(GLSTATE_BLEND, 1);
        dglEnable(GL_ALPHA_TEST);
    }

    dglColor4ubv((byte*)&section->color1);

    dglNormalPointer(GL_FLOAT, sizeof(float), section->normals);
    dglTexCoordPointer(2, GL_FLOAT, 0, section->coords);
    dglVertexPointer(3, GL_FLOAT, sizeof(vec3_t), section->xyz);

    GL_BindTexture(Tex_CacheTextureFile(
        section->texpath, GL_REPEAT, section->flags & MDF_MASKED));

    if(has_GL_EXT_compiled_vertex_array)
    {
        dglLockArraysEXT(0, (section->numtris / 3) * 2);
    }

    dglDrawElements(GL_TRIANGLES, section->numtris, GL_UNSIGNED_SHORT, section->tris);

    if(has_GL_EXT_compiled_vertex_array)
    {
        dglUnlockArraysEXT();
    }

    if(section->flags & MDF_MASKED)
    {
        GL_SetState(GLSTATE_BLEND, 0);
        dglDisable(GL_ALPHA_TEST);
    }

    if(section->flags & MDF_NOCULLFACES)
    {
        GL_SetState(GLSTATE_CULL, true);
    }
}

//
// R_DrawTestModel
//

static void R_DrawTestModel(const char *file)
{
    kmodel_t *model;
    unsigned int i;

    if(!(model = Mdl_Load(file)))
    {
        return;
    }

    dglCullFace(GL_BACK);
    dglEnable(GL_DEPTH_TEST);

    dglDisableClientState(GL_COLOR_ARRAY);

    for(i = 0; i < model->nodes[0].meshes[0].numsections; i++)
    {
        R_DrawSection(&model->nodes[0].meshes[0].sections[i]);
    }

    dglEnableClientState(GL_COLOR_ARRAY);
    dglDisable(GL_DEPTH_TEST);
    dglCullFace(GL_FRONT);
}

//
// R_DrawFrame
//

void R_DrawFrame(void)
{
    mtx_t mtx;

    GL_ClearView(0xFF3f3f3f);
    dglMatrixMode(GL_PROJECTION);
    dglLoadIdentity();
    Mtx_ViewFrustum(video_width, video_height, cl_fov.value, 0.1f);
    dglMatrixMode(GL_MODELVIEW);
    dglLoadIdentity();
    Mtx_Identity(mtx);
    Mtx_SetTranslation(mtx, 0, -1024, -1500);
    dglMultMatrixf(mtx);

    R_DrawTestCube();
    R_DrawTestModel("models/mdl320/mdl320.kmesh");

    GL_SetOrtho();
}

//
// R_FinishFrame
//

void R_FinishFrame(void)
{
    IN_UpdateGrab();
    GL_SwapBuffers();
}

//
// R_Init
//

void R_Init(void)
{
    Mdl_Init();
}
