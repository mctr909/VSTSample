#ifndef __HEADER_RIFF__
#define __HEADER_RIFF__

#include <stdio.h>
#include <vector>
#include <string>

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int int32;
typedef short int16;
typedef char int8;

class RIFF {
public:
    typedef struct CHUNK {
        char id[4];
        uint32 size;
        uint8* data;
    } CHUNK;

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
    void Save(wchar_t *path);
    void AddInfo(const char *id, std::string text);
    void AddInfo(const char *id, FILE *fp, uint32 size);
    uint8* AddChunk(const char *id, uint8 *data, uint32 size);
    uint8* AddChunk(const char *id, FILE *fp, uint32 size);

protected:
    void Load(wchar_t *path);
    void Load(FILE *fp, uint32 size);
    uint32 Write(FILE *fp);
    virtual void WriteChunk() { }
    virtual bool CheckFile(char *type);
    virtual void LoadChunk(FILE *fp, char *type, uint32 size);
    virtual void LoadInfo(FILE *fp, char *type, uint32 size);
    virtual void LoadList(FILE *fp, char *type, uint32 size);
    virtual const char* ID() { return (char*)&ZERO_PAD; }

private:
    void loadInfo(FILE *fp, uint32 size);
    uint32 writeChunk(FILE *fp);
    uint32 writeInfo(FILE *fp);
};

#endif //__HEADER_RIFF__
