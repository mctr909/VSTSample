#include <string.h>
#include "instruments.h"

void Inst::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("colh", type)) {
        AddChunk("colh", fp, size);
        return;
    }
    if (0 == strcmp("ptbl", type)) {
        AddChunk("ptbl", fp, size);
        return;
    }
    if (0 == strcmp("vers", type)) {
        fseek(fp, size, SEEK_CUR);
        return;
    }
    if (0 == strcmp("msyn", type)) {
        fseek(fp, size, SEEK_CUR);
        return;
    }
    if (0 == strcmp("DLID", type)) {
        fseek(fp, size, SEEK_CUR);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void Inst::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("lins", type)) {
        List.push_back(new LINS(fp, size));
        return;
    }
    if (0 == strcmp("wvpl", type)) {
        List.push_back(new WVPL(fp, size));
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void LINS::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("ins ", type)) {
        List.push_back(new INS_(fp, size));
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void INS_::LoadInfo(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("INAM", type)) {
        AddInfo("INAM", fp, size);
        return;
    }
    if (0 == strcmp("ICAT", type)) {
        AddInfo("ICAT", fp, size);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void INS_::WriteChunk() {
    AddChunk("insh", (uint8*)&Header, sizeof(Header));
}

void INS_::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("insh", type)) {
        auto hd = AddChunk("insh", fp, size);
        memcpy_s(&Header, sizeof(Header), hd, size);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void INS_::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("lrgn", type)) {
        List.push_back(new LRGN(fp, size));
        return;
    }
    if (0 == strcmp("lart", type) || 0 == strcmp("lar2", type)) {
        List.push_back(new LART(fp, size));
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void LRGN::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("rgn ", type)) {
        List.push_back(new RGN_(fp, size));
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void RGN_::WriteChunk() {
    AddChunk("rgnh", (uint8*)&Header, sizeof(Header));
}

void RGN_::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("rgnh", type)) {
        auto hd = AddChunk("rgnh", fp, size);
        memcpy_s(&Header, sizeof(Header), hd, size);
        return;
    }
    if (0 == strcmp("wlnk", type)) {
        AddChunk("wlnk", fp, size);
        return;
    }
    if (0 == strcmp("wsmp", type)) {
        AddChunk("wsmp", fp, size);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void RGN_::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("lart", type) || 0 == strcmp("lar2", type)) {
        List.push_back(new LART(fp, size));
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void LART::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("art1", type) || 0 == strcmp("art2", type)) {
        List.push_back(new ART_(fp, size));
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

ART_::ART_(FILE *fp, uint32 size) {
    fseek(fp, size, SEEK_CUR);
}

ART_::~ART_() { }

void WVPL::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("wave", type)) {
        List.push_back(new WAVE(fp, size));
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void WAVE::LoadInfo(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("INAM", type)) {
        AddInfo("INAM", fp, size);
        return;
    }
    if (0 == strcmp("ICAT", type)) {
        AddInfo("ICAT", fp, size);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void WAVE::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("fmt ", type)) {
        AddChunk("fmt ", fp, size);
        return;
    }
    if (0 == strcmp("data", type)) {
        AddChunk("data", fp, size);
        return;
    }
    if (0 == strcmp("wsmp", type)) {
        AddChunk("wsmp", fp, size);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}
