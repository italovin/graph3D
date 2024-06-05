#ifndef GL_API_VERSIONS_H
#define GL_API_VERSIONS_H

enum class GLApiVersion{
    Undefined = 0,
    V200 = 200, 
    V210 = 210, 
    V300 = 300, 
    V310 = 310,
    V320 = 320,
    V330 = 330,
    V400 = 400,
    V410 = 410,
    V420 = 420,
    V430 = 430,
    V440 = 440,
    V450 = 450,
    V460 = 460,
};

struct GLApiVersionStruct{
    static GLApiVersion GetVersionFromInteger(int versionInt){
        switch(versionInt){
            case 200: return GLApiVersion::V200;
            case 210: return GLApiVersion::V210;
            case 300: return GLApiVersion::V300;
            case 310: return GLApiVersion::V310;
            case 320: return GLApiVersion::V320;
            case 330: return GLApiVersion::V330;
            case 400: return GLApiVersion::V400;
            case 410: return GLApiVersion::V410;
            case 420: return GLApiVersion::V420;
            case 430: return GLApiVersion::V430;
            case 440: return GLApiVersion::V440;
            case 450: return GLApiVersion::V450;
            case 460: return GLApiVersion::V460;
            default: return GLApiVersion::Undefined;
        }
    }
};

#endif