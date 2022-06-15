#ifndef __HEADER_RIFF__
#define __HEADER_RIFF__

#include <stdio.h>
#include <vector>
#include <string>

#include <pluginterfaces/base/ftypes.h>

namespace Steinberg {
    class RIFF {
    public:
        typedef struct CHUNK {
            char id[4];
            uint32 size;
            uint8* data;
        } CHUNK;

        typedef struct WAVE_FMT {
            uint16 tag;
            uint16 channels;
            uint32 sampleRate;
            uint32 bytesPerSec;
            uint16 blockAlign;
            uint16 bits;
        } WAVE_FMT;

    private:
        const char RIFF_ID[4] = { 'R', 'I', 'F', 'F' };
        const char LIST_ID[4] = { 'L', 'I', 'S', 'T' };
        const char INFO_ID[4] = { 'I', 'N', 'F' ,'O' };
        const uint32 ZERO_PAD = 0;

    public:
        std::vector<CHUNK> Chunks;
        std::vector<CHUNK> Info;
        std::vector<RIFF*> List;

    public:
        RIFF() { }
        ~RIFF() { Clear(); }

        void Clear();
        void Save(wchar_t* path);
        void AddInfo(const char* id, std::string text);
        void AddInfo(const char* id, FILE* fp, uint32 size);
        uint8* AddChunk(const char* id, uint8* data, uint32 size);
        uint8* AddChunk(const char* id, FILE* fp, uint32 size);
        void RemoveChunk(const char* id);

    protected:
        void Load(wchar_t* path);
        void Load(FILE* fp, uint32 size);
        uint32 Write(FILE* fp);
        virtual bool CheckFile(char* type);
        virtual void LoadChunk(FILE* fp, char* type, uint32 size);
        virtual void LoadInfo(FILE* fp, char* type, uint32 size);
        virtual void LoadList(FILE* fp, char* type, uint32 size);
        virtual const char* ID() { return (char*)&ZERO_PAD; }

    private:
        void loadInfo(FILE* fp, uint32 size);
        uint32 writeChunk(FILE* fp);
        uint32 writeInfo(FILE* fp);
    };
}

#endif //__HEADER_RIFF__
