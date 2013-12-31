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
// DESCRIPTION: String Class
//
//-----------------------------------------------------------------------------

#include <ctype.h>

#ifndef EDITOR
#include "common.h"
#else
#include "editorCommon.h"
#endif
#include "kstring.h"

//
// kexStr::Init
//

void kexStr::Init(void) {
    length = 0;
    bufferLength = STRING_DEFAULT_SIZE;
    charPtr = defaultBuffer;
    charPtr[0] = '\0';
}

//
// kexStr::CheckSize
//

void kexStr::CheckSize(int size, bool bKeepString) {
    if(size <= bufferLength) {
        return;
    }

    Resize(size, bKeepString);
}

//
// kexStr::CopyNew
//

void kexStr::CopyNew(const char *string, int len) {
    CheckSize(len+1, false);
    strcpy(charPtr, string);
    length = len;
}

//
// kexStr::kexStr
//

kexStr::kexStr(void) {
    Init();
}

//
// kexStr::kexStr
//

kexStr::kexStr(const char *string) {
    Init();

    if(string == NULL) {
        return;
    }

    CopyNew(string, strlen(string));
}

//
// kexStr::kexStr
//

kexStr::kexStr(const char *string, const int length) {
    Init();

    if(string == NULL) {
        return;
    }

    CopyNew(string, length);
}

//
// kexStr::kexStr
//

kexStr::kexStr(const kexStr &string) {
    Init();

    if(string.charPtr == NULL) {
        return;
    }

    CopyNew(string.charPtr, string.Length());
}

//
// kexStr::~kexStr
//

kexStr::~kexStr(void) {
    if(charPtr != defaultBuffer) {
        delete[] charPtr;
        charPtr = defaultBuffer;
    }

    charPtr[0] = '\0';
    length = 0;
}

//
// kexStr::Concat
//

kexStr &kexStr::Concat(const char *string) {
    return Concat(string, strlen(string));
}

//
// kexStr::Concat
//

kexStr &kexStr::Concat(const char *string, int len) {
    CheckSize((length + len)+1, true);

    for(int i = 0; i < len; i++) {
        charPtr[length+i] = string[i];
    }

    length += len;
    charPtr[length] = '\0';
    
    return *this;
}

//
// kexStr::Copy
//

kexStr &kexStr::Copy(const kexStr &src, int len) {
    int i = 0;
    const char *p = src;
    CheckSize((length + len)+1, true);
    
    while((len--) >= 0) {
        charPtr[i] = p[i];
        i++;
    }

    return *this;
}

//
// kexStr::Copy
//

kexStr &kexStr::Copy(const kexStr &src) {
    return Copy(src, src.Length());
}

//
// kexStr::operator=
//

kexStr &kexStr::operator=(const kexStr &str) {
    int len = str.Length();
    
    CheckSize(len+1, false);
    strncpy(charPtr, str.charPtr, len);
    length = len;
    charPtr[length] = '\0';

    return *this;
}

//
// kexStr::operator=
//

kexStr &kexStr::operator=(const char *str) {
    int len = strlen(str);
    
    CheckSize(len+1, false);
    strncpy(charPtr, str, len);
    length = len;
    charPtr[length] = '\0';

    return *this;
}

//
// kexStr::operator=
//

kexStr &kexStr::operator=(const bool b) {
    const char *str = b ? "true" : "false";
    int len = strlen(str);
    
    CheckSize(len+1, false);
    strncpy(charPtr, str, len);
    length = len;
    charPtr[length] = '\0';

    return *this;
}

//
// kexStr::operator+
//

kexStr kexStr::operator+(const kexStr &str) {
    kexStr out(*this);
    
    return out.Concat(str.c_str());
}

//
// kexStr::operator+
//

kexStr kexStr::operator+(const char *str) {
    kexStr out(*this);
    
    return out.Concat(str);
}

//
// kexStr::operator+
//

kexStr kexStr::operator+(const bool b) {
    kexStr out(*this);
    
    return out.Concat(b ? "true" : "false");
}

//
// kexStr::operator+
//

kexStr kexStr::operator+(const int i) {
    kexStr out(*this);

    char tmp[64];
    sprintf(tmp, "%i", i);
    
    return out.Concat(tmp);
}

//
// kexStr::operator+
//

kexStr kexStr::operator+(const float f) {
    kexStr out(*this);

    char tmp[64];
    sprintf(tmp, "%f", f);
    
    return out.Concat(tmp);
}

//
// kexStr::operator+=
//

kexStr &kexStr::operator+=(const kexStr &str) {
    return Concat(str.c_str());
}

//
// kexStr::operator+=
//

kexStr &kexStr::operator+=(const char *str) {
    return Concat(str);
}

//
// kexStr::operator+=
//

kexStr &kexStr::operator+=(const char c) {
    return Concat(&c);
}

//
// kexStr::operator+=
//

kexStr &kexStr::operator+=(const bool b) {
    return Concat(b ? "true" : "false");
}

//
// kexStr::operator[]
//

const char kexStr::operator[](int index) const {
    assert(index >= 0 && index < length);
    return charPtr[index];
}

//
// kexStr::Resize
//

void kexStr::Resize(int size, bool bKeepString) {

    if(size <= 0)
        return;

    int newsize = size + ((32 - (size & 31)) & 31);
    char *newbuffer = new char[newsize];

    if(bKeepString) {
        strncpy(newbuffer, charPtr, length);
    }

    if(charPtr != defaultBuffer) {
        delete[] charPtr;
    }

    charPtr = newbuffer;
    bufferLength = newsize;
}

//
// kexStr::IndexOf
//

int kexStr::IndexOf(const char *pattern) const {
    int patlen = strlen(pattern);
    int i = 0;
    int j = 0;
    int index = -1;

    while(i + j < Length()) {
        if(charPtr[i + j] == pattern[j]) {
            if(++j == patlen) {
                return i;
            }
        }
        else {
            i++;
            j = 0;
        }
    }

    return -1;
}

//
// kexStr::IndexOf
//

int kexStr::IndexOf(const char *string, const char *pattern) {
    int patlen = strlen(pattern);
    int i = 0;
    int j = 0;
    int index = -1;

    while(i + j < (int)strlen(string)) {
        if(string[i + j] == pattern[j]) {
            if(++j == patlen) {
                return i;
            }
        }
        else {
            i++;
            j = 0;
        }
    }

    return -1;
}

//
// kexStr::IndexOf
//

int kexStr::IndexOf(const kexStr &pattern) const {
    return IndexOf(pattern.c_str());
}

//
// kexStr::NormalizeSlashes
//

kexStr &kexStr::NormalizeSlashes(void) {
    for(int i = 0; i < length; i++) {
        if((charPtr[i] == '/' || charPtr[i] == '\\') && charPtr[i] != DIR_SEPARATOR) {
            charPtr[i] = DIR_SEPARATOR;
        }
    }
    
    return *this;
}

//
// kexStr::StripPath
//

kexStr &kexStr::StripPath(void) {
    int pos = 0;
    int i = 0;

    pos = length;

    for(i = length - 1; charPtr[i] != '\\' && charPtr[i] != '/'; i--, pos--) {
        if(pos <= 0) {
            return *this;
        }
    }
    length = length - pos;
    for(i = 0; i < length; i++) {
        charPtr[i] = charPtr[pos+i];
    }
    
    CheckSize(length, true);
    charPtr[length] = '\0';
    return *this;
}

//
// kexStr::StripExtension
//

kexStr &kexStr::StripExtension(void) {
    int pos = IndexOf(".");
    
    if(pos == -1) {
        return *this;
    }
    
    length = pos;
    CheckSize(length, true);
    charPtr[length] = '\0';
    
    return *this;
}

//
// kexStr::Hash
//

int kexStr::Hash(void) {
    unsigned int hash   = 0;
    char *str           = (char*)charPtr;
    char c;

    while((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash & (MAX_HASH-1);
}

//
// kexStr::Hash
//

int kexStr::Hash(const char *s) {
    unsigned int hash   = 0;
    char *str           = (char*)s;
    char c;

    while((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash & (MAX_HASH-1);
}

//
// kexStr::Substr
//

kexStr kexStr::Substr(int start, int len) const {
    kexStr str;
    int l = Length();
    
    if(l <= 0 || start >= l)
        return str;
        
    if(start + len >= l)
        len = l - start;
        
    return str.Concat((const char*)&charPtr[start], len);
}

//
// kexStr::Split
//

void kexStr::Split(kexStrListMem &list, const char seperator) {
    int splitLen = 0;
    int startOffs = 0;
    for(int i = 0; i < length; i++) {
        if(charPtr[i] == seperator) {
            if(splitLen == 0) {
                continue;
            }
            
            list.Push(new kexStr(&charPtr[startOffs], splitLen));
            startOffs += (splitLen+1);
            splitLen = 0;
            continue;
        }
        
        splitLen++;
    }

    if(splitLen != 0 && startOffs != 0) {
        list.Push(new kexStr(&charPtr[startOffs], splitLen));
    }
}

//
// kexStr::Atoi
//

int kexStr::Atoi(void) {
    return atoi(charPtr);
}

//
// kexStr::ToUpper
//

kexStr &kexStr::ToUpper(void) {
    char c;
    for(int i = 0; i < length; i++) {
        c = charPtr[i];
        if(c >= 'a' && c <= 'z')
            c -= 'a'-'A';
        charPtr[i] = c;
    }
    
    return *this;
}

//
// kexStr::ToLower
//

kexStr &kexStr::ToLower(void) {
    char c;
    for(int i = 0; i < length; i++) {
        c = charPtr[i];
        if(c >= 'A' && c <= 'Z')
            c += 32;
        charPtr[i] = c;
    }
    
    return *this;
}

//
// kexStr::CompareCase
//

bool kexStr::CompareCase(const char *s1, const char *s2) {
    while(*s1 && *s2) {
        if(*s1 != *s2) {
            return (*s2 - *s1) != 0;
        }
        s1++;
        s2++;
    }
    if(*s1 != *s2) {
        return (*s2 - *s1) != 0;
    }
        
    return false;
}

//
// kexStr::CompareCase
//

bool kexStr::CompareCase(const kexStr &a, const kexStr &b) {
    return CompareCase(a.c_str(), b.c_str());
}

//
// kexStr::Compare
//

bool kexStr::Compare(const char *s1, const char *s2) {
    const byte *us1 = (const byte*)s1;
    const byte *us2 = (const byte*)s2;

    while(tolower(*us1) == tolower(*us2)) {
        if(*us1++ == '\0') {
            return false;
        }

        us2++;
    }

    return (tolower(*us1) - tolower(*us2)) != 0;
}

//
// kexStr::Compare
//

bool kexStr::Compare(const kexStr &a, const kexStr &b) {
    return Compare(a.c_str(), b.c_str());
}

#ifndef EDITOR
//
// kexStr::ObjectConstruct
//

void kexStr::ObjectConstruct(kexStr *thisstring) {
    new(thisstring)kexStr();
}

//
// kexStr::ObjectConstructCopy
//

void kexStr::ObjectConstructCopy(const kexStr &in, kexStr *thisstring) {
    new(thisstring)kexStr(in);
}

//
// kexStr::ObjectFactory
//

kexStr kexStr::ObjectFactory(unsigned int byteLength, const char *s) {
    return kexStr(s);
}

//
// kexStr::ObjectDeconstruct
//

void kexStr::ObjectDeconstruct(kexStr *thisstring) {
    thisstring->~kexStr();
}
#endif
