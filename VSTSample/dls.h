#ifndef __HEADER_DLS__
#define __HEADER_DLS__

#include "riff.h"
#include "dls_struct.h"

using std::vector;

namespace Steinberg {
    class LINS;
    class LRGN;
    class LART;
    class WVPL;
    class INS_;
    class RGN_;
    class WAVE;

    class DLS : public RIFF {
    private:
        vector<RIFF*>* mpInstList = NULL;
        vector<RIFF*>* mpWaveList = NULL;

    public:
        static DLS* Instance;
        DLS(wchar_t* path) { Load(path); }
        ~DLS() { Clear(); }
        INS_* GetInst(bool is_drum, uint8 bank_msb, uint8 bank_lsb, uint8 prog_num);
        WAVE* GetWave(RGN_* rgn);

    protected:
        const char mId[4] = { 'D', 'L', 'S', ' ' };
        const char* ID() override { return mId; }
        bool CheckFile(char* type) override { return 0 == strcmp("DLS ", type); }
        void LoadChunk(FILE* fp, char* type, uint32 size) override;
        void LoadList(FILE* fp, char* type, uint32 size) override;
    };

    class LINS : public RIFF {
    public:
        LINS(FILE* fp, uint32 size) { Load(fp, size); }
        ~LINS() { Clear(); }

    protected:
        const char mId[4] = { 'l', 'i', 'n', 's' };
        const char* ID() override { return mId; }
        void LoadList(FILE* fp, char* type, uint32 size) override;
    };

    class LRGN : public RIFF {
    public:
        LRGN(FILE* fp, uint32 size) { Load(fp, size); }
        ~LRGN() { Clear(); }

    protected:
        const char mId[4] = { 'l', 'r', 'g', 'n' };
        const char* ID() override { return mId; }
        void LoadList(FILE* fp, char* type, uint32 size) override;
    };

    class LART : public RIFF {
    public:
        uint32 Count = 0;
        DLS_CONN* pConn = NULL;
        LART(FILE* fp, uint32 size) { Load(fp, size); }
        ~LART() { Clear(); }

    protected:
        const char mId[4] = { 'l', 'a', 'r', 't' };
        const char* ID() override { return mId; }
        void LoadChunk(FILE* fp, char* type, uint32 size) override;
    };

    class WVPL : public RIFF {
    public:
        WVPL(FILE* fp, uint32 size) { Load(fp, size); }
        ~WVPL() { Clear(); }

    protected:
        const char mId[4] = { 'w', 'v', 'p', 'l' };
        const char* ID() override { return mId; }
        void LoadList(FILE* fp, char* type, uint32 size) override;
    };

    class INS_ : public RIFF {
    public:
        DLS_INSH* pHeader = NULL;
        vector<RIFF*>* pRegions = NULL;
        LART* pArtList = NULL;

    public:
        INS_(FILE* fp, uint32 size) { Load(fp, size); }
        ~INS_() { Clear(); }
        vector<RGN_*> GetRegions(uint8 note_num, uint8 velo);

    protected:
        const char mId[4] = { 'i', 'n', 's', ' ' };
        const char* ID() override { return mId; }
        void LoadInfo(FILE* fp, char* type, uint32 size) override;
        void LoadChunk(FILE* fp, char* type, uint32 size) override;
        void LoadList(FILE* fp, char* type, uint32 size) override;
    };

    class RGN_ : public RIFF {
    public:
        DLS_RGNH* pHeader = NULL;
        DLS_WLNK* pWlnk = NULL;
        DLS_WSMP* pWsmp = NULL;
        DLS_LOOP* pLoop = NULL;
        LART* pArtList = NULL;

    public:
        RGN_(FILE* fp, uint32 size) { Load(fp, size); }
        ~RGN_() { Clear(); }

    protected:
        const char mId[4] = { 'r', 'g', 'n', ' ' };
        const char* ID() override { return mId; }
        void LoadChunk(FILE* fp, char* type, uint32 size) override;
        void LoadList(FILE* fp, char* type, uint32 size) override;
    };

    class WAVE : public RIFF {
    public:
        WAVE_FMT* pFmt = NULL;
        DLS_WSMP* pWsmp = NULL;
        DLS_LOOP* pLoop = NULL;
        int16* pWave = NULL;
        uint32 Size = 0;

    public:
        WAVE(FILE* fp, uint32 size) { Load(fp, size); }
        ~WAVE() { Clear(); }

    protected:
        const char mId[4] = { 'w', 'a', 'v', 'e' };
        const char* ID() override { return mId; }
        void LoadInfo(FILE* fp, char* type, uint32 size) override;
        void LoadChunk(FILE* fp, char* type, uint32 size) override;
    };
}

#endif //__HEADER_DLS__
