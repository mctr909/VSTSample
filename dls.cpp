#include <string.h>
#include "dls.h"

void DLS::LoadChunk(FILE *fp, char *type, uint32 size) {
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

void DLS::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("lins", type)) {
        auto lins = new LINS(fp, size);
        List.push_back(lins);
        mpInstList = &lins->List;
        return;
    }
    if (0 == strcmp("wvpl", type)) {
        auto wvpl = new WVPL(fp, size);
        List.push_back(wvpl);
        mpWaveList = &wvpl->List;
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

INS_* DLS::GetInst(bool is_drum, uint8 bank_msb, uint8 bank_lsb, uint8 prog_num) {
    for (int i = 0; i < mpInstList->size(); i++) {
        auto ins = (INS_*)(*mpInstList)[i];
        auto hd = ins->pHeader;
        if (is_drum) {
            if ((hd->bankFlags & 0x80) &&
                hd->bankMSB == bank_msb &&
                hd->bankLSB == bank_lsb &&
                hd->progNum == prog_num) {
                return ins;
            }
        } else {
            if (!(hd->bankFlags & 0x80) &&
                hd->bankMSB == bank_msb &&
                hd->bankLSB == bank_lsb &&
                hd->progNum == prog_num) {
                return ins;
            }
        }
    }
    for (int i = 0; i < mpInstList->size(); i++) {
        auto ins = (INS_*)(*mpInstList)[i];
        auto hd = ins->pHeader;
        if (is_drum) {
            if ((hd->bankFlags & 0x80) &&
                hd->bankMSB == 0 &&
                hd->bankLSB == 0 &&
                hd->progNum == prog_num) {
                return ins;
            }
        } else {
            if (!(hd->bankFlags & 0x80) &&
                hd->bankMSB == 0 &&
                hd->bankLSB == 0 &&
                hd->progNum == prog_num) {
                return ins;
            }
        }
    }
    for (int i = 0; i < mpInstList->size(); i++) {
        auto ins = (INS_*)(*mpInstList)[i];
        auto hd = ins->pHeader;
        if (is_drum) {
            if ((hd->bankFlags & 0x80) &&
                hd->bankMSB == 0 &&
                hd->bankLSB == 0 &&
                hd->progNum == 0) {
                return ins;
            }
        } else {
            if (!(hd->bankFlags & 0x80) &&
                hd->bankMSB == 0 &&
                hd->bankLSB == 0 &&
                hd->progNum == 0) {
                return ins;
            }
        }
    }
    return NULL;
}

WAVE* DLS::GetWave(RGN_ *rgn) {
    return (WAVE*)(*mpWaveList)[rgn->pWlnk->tableIndex];
}

void LINS::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("ins ", type)) {
        List.push_back(new INS_(fp, size));
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

void LART::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("art1", type) || 0 == strcmp("art2", type)) {
        auto ptr = AddChunk("art1", fp, size);
        Count = *(uint32*)(ptr + 4);
        pConn = (DLS_CONN*)(ptr + 8);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void WVPL::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("wave", type)) {
        List.push_back(new WAVE(fp, size));
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

void INS_::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("insh", type)) {
        pHeader = (DLS_INSH*)AddChunk("insh", fp, size);
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void INS_::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("lrgn", type)) {
        auto lrgn = new LRGN(fp, size);
        List.push_back(lrgn);
        pRegions = &lrgn->List;
        return;
    }
    if (0 == strcmp("lart", type) || 0 == strcmp("lar2", type)) {
        auto lart = new LART(fp, size);
        List.push_back(lart);
        pArtList = lart;
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

vector<RGN_*> INS_::GetRegions(uint8 note_num, uint8 velo) {
    vector<RGN_*> ret;
    for (int i = 0; i < pRegions->size(); i++) {
        auto rgn = (RGN_*)(*pRegions)[i];
        auto hd = rgn->pHeader;
        if (hd->keyLow <= note_num && note_num <= hd->keyHigh &&
            hd->velocityLow <= velo && velo <= hd->velocityHigh) {
            ret.push_back(rgn);
        }
    }
    return ret;
}

void RGN_::LoadChunk(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("rgnh", type)) {
        pHeader = (DLS_RGNH*)AddChunk("rgnh", fp, size);
        return;
    }
    if (0 == strcmp("wlnk", type)) {
        pWlnk = (DLS_WLNK*)AddChunk("wlnk", fp, size);
        return;
    }
    if (0 == strcmp("wsmp", type)) {
        auto ptr = AddChunk("wsmp", fp, size);
        pWsmp = (DLS_WSMP*)ptr;
        if (0 < pWsmp->loopCount) {
            pLoop = (DLS_LOOP*)(ptr + sizeof(DLS_WSMP));
        } else {
            pLoop = NULL;
        }
        return;
    }
    fseek(fp, size, SEEK_CUR);
}

void RGN_::LoadList(FILE *fp, char *type, uint32 size) {
    if (0 == strcmp("lart", type) || 0 == strcmp("lar2", type)) {
        auto lart = new LART(fp, size);
        List.push_back(lart);
        pArtList = lart;
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
        pFmt = (WAVE_FMT*)AddChunk("fmt ", fp, size);
        return;
    }
    if (0 == strcmp("data", type)) {
        pWave = (int16*)AddChunk("data", fp, size);
        return;
    }
    if (0 == strcmp("wsmp", type)) {
        auto ptr = AddChunk("wsmp", fp, size);
        pWsmp = (DLS_WSMP*)ptr;
        if (0 < pWsmp->loopCount) {
            pLoop = (DLS_LOOP*)(ptr + sizeof(DLS_WSMP));
        } else {
            pLoop = NULL;
        }
        return;
    }
    fseek(fp, size, SEEK_CUR);
}
