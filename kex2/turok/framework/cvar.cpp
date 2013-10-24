// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 Samuel Villarreal
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
// DESCRIPTION: Console cvar functionality
//
//-----------------------------------------------------------------------------

#include "common.h"
#include "zone.h"

kexCvarManager cvarManager;

//
// FCmd_ListVars
//

static void FCmd_ListVars(void) {
    kexCvar *var;

    common.CPrintf(COLOR_GREEN, "Available cvars:\n");

    for(var = cvarManager.GetFirst(); var; var = var->GetNext()) {
        common.CPrintf(COLOR_CYAN, "%s\n", var->GetName());
    }
}

//
// FCmd_Seta
//

static void FCmd_Seta(void) {
    if(command.GetArgc() < 2) {
        return;
    }

    cvarManager.Set(command.GetArgv(1), command.GetArgv(2));
}

//
// kexCvar::kexCvar
//

kexCvar::kexCvar(const char *name, int flags, char *value, const char *description) {
    Setup(name, value, description, flags, 0, 1);
}

//
// kexCvar::kexCvar
//

kexCvar::kexCvar(const char *name, int flags, char *value,
                 float min, float max, const char *description) {
    Setup(name, value, description, flags, min, max);
}

//
// kexCvar::FreeStringValue
//

void kexCvar::FreeStringValue(void) {
    bModified ? Z_Free((char*)value) : value = NULL;
}

//
// kexCvar::SetNewStringValue
//

void kexCvar::SetNewStringValue(const char *string) {
    value = (char*)(Z_Malloc(strlen(string)+1, PU_STATIC, 0));
    strcpy(value, string);

    bModified = true;
}

//
// kexCvar::Set
//

void kexCvar::Set(const char *string)
{
    FreeStringValue();
    SetNewStringValue(string);
}

//
// kexCvar::Set
//

void kexCvar::Set(float value) {
    Set(kva("%f", value));
}

//
// kexCvar::Set
//

void kexCvar::Set(int value) {
    Set(kva("%i", value));
}

//
// kexCvarManager::Get
//

kexCvar *kexCvarManager::Get(const char *name) {
    kexCvar *var;
    
    for(var = first; var; var = var->GetNext()) {
        if(!strcmp(name, var->GetName()))
            return var;
    }
    
    return NULL;
}

//
// kexCvarManager::Set
//

void kexCvarManager::Set(const char *var_name, const char *value) {
    kexCvar *var;
    
    var = Get(var_name);
    if(!var) {
        // there is an error in C code if this happens
        common.Warning("Cvar_Set: variable %s not found\n", var_name);
        return;
    }

    var->Set(value);
}

//
// kexCvarManager::Set
//

void kexCvarManager::Set(const char *var_name, float value) {
    Set(var_name, kva("%f", value));
}

//
// kexCvarManager::Set
//

void kexCvarManager::Set(const char *var_name, int value) {
    Set(var_name, kva("%i", value));
}

//
// kexCvarManager::AutoComplete
//

void kexCvarManager::AutoComplete(const char *partial) {
    kexCvar     *cvar;
    int         len;
    bool        match = false;
    
    strlwr((char*)partial);
    len = strlen(partial);
    
    if(!len) {
        return;
    }
    
    // check functions
    for(cvar = first; cvar; cvar = cvar->GetNext()) {
        if(!strncmp(partial, cvar->GetName(), len)) {
            if(!match) {
                match = true;
                common.Printf("\n");
            }

            // print all matching cvars
            common.Printf("%s\n", cvar->GetName());
        }
    }
}

//
// kexCvarManager::Register
//

void kexCvarManager::Register(kexCvar *variable) {
    if(first == NULL)
        first = variable;

    if(next != NULL)
        next->SetNext(variable);

    next = variable;
}

//
// kexCvarManager::WriteToFile
//

void kexCvarManager::WriteToFile(FILE *file) {
    kexCvar *cvar;

    for(cvar = first; cvar; cvar = cvar->GetNext()) {
        if(!(cvar->GetFlags() & CVF_CONFIG))
            continue;

        fprintf(file, "seta %s \"%s\"\n", cvar->GetName(), cvar->GetValue());
    }
}

//
// kexCvarManager::Init
//

void kexCvarManager::Init(void) {
    command.Add("listvars", FCmd_ListVars);
    command.Add("seta", FCmd_Seta);

    int p;

    if(p = common.CheckParam("-setvar")) {
        p++;

        while(p != myargc && myargv[p][0] != '-') {
            char *name;
            char *value;

            name = myargv[p++];
            value = myargv[p++];

            cvarManager.Set(name, value);
        }
    }

    common.Printf("Cvar System Initialized\n");
}

//
// kexCvarManager::Shutdown
//

void kexCvarManager::Shutdown(void) {
    kexCvar *cvar = first;
    while(1) {
        if(cvar == NULL)
            break;

        kexCvar *tmpVar = cvar->GetNext();

        if(cvar->GetFlags() & CVF_ALLOCATED)
            delete cvar;

        cvar = tmpVar;
    }
}

//
// kexCvar::Setup
//

void kexCvar::Setup(const char *name, char *value,
                    const char *description, int flags, float min, float max) {
    this->name          = name;
    this->value         = value;
    this->flags         = flags;
    this->description   = description;
    this->defaultValue  = value;
    this->min           = min;
    this->max           = max;
    this->bModified     = false;
    this->next          = NULL;

    cvarManager.Register(this);
}
