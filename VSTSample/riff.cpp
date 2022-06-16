#include <string.h>
#include "riff.h"

namespace Steinberg {
    void RIFF::Clear() {
        for (uint32 i = 0; i < Chunks.size(); i++) {
            free(Chunks[i].data);
        }
        Chunks.clear();
        for (uint32 i = 0; i < Info.size(); i++) {
            free(Info[i].data);
        }
        Info.clear();
        for (uint32 i = 0; i < List.size(); i++) {
            delete List[i];
        }
        List.clear();
    }

    void RIFF::Save(wchar_t* path) {
        FILE* fp = NULL;
        _wfopen_s(&fp, path, L"wb");
        if (NULL == fp) {
            return;
        }

        fwrite(RIFF_ID, 4, 1, fp);
        fwrite(&ZERO_PAD, 4, 1, fp);
        fwrite(ID(), 4, 1, fp);

        uint32 size = 4;
        size += writeChunk(fp);
        size += writeInfo(fp);

        for (uint32 i = 0; i < List.size(); i++) {
            size += List[i]->Write(fp);
        }

        fseek(fp, 4, SEEK_SET);
        fwrite(&size, 4, 1, fp);

        fclose(fp);
    }

    void RIFF::AddInfo(const char* id, std::string text) {
        CHUNK* old_info = NULL;
        for (uint32 i = 0; i < Info.size(); i++) {
            if (0 == memcmp(id, Info[i].id, 4)) {
                free(Info[i].data);
                old_info = &Info[i];
            }
        }

        auto info = CHUNK();
        memcpy(info.id, id, 4);
        info.size = text.length();
        auto pad_size = info.size + (0 < info.size % 2 ? 1 : 0);
        info.data = (uint8*)calloc(1, pad_size);
        memcpy_s(info.data, pad_size, text.c_str(), info.size);

        if (NULL == old_info) {
            Info.push_back(info);
        }
        else {
            memcpy(old_info, &info, sizeof(info));
        }
    }

    void RIFF::AddInfo(const char* id, FILE* fp, uint32 size) {
        CHUNK* old_info = NULL;
        for (uint32 i = 0; i < Info.size(); i++) {
            if (0 == memcmp(id, Info[i].id, 4)) {
                free(Info[i].data);
                old_info = &Info[i];
            }
        }

        auto info = CHUNK();
        memcpy(info.id, id, 4);
        info.size = size;
        info.data = (uint8*)calloc(1, size);
        fread_s(info.data, size, size, 1, fp);

        if (NULL == old_info) {
            Info.push_back(info);
        }
        else {
            memcpy(old_info, &info, sizeof(info));
        }
    }

    uint8* RIFF::AddChunk(const char* id, uint8* data, uint32 size) {
        CHUNK* old_chunk = NULL;
        for (uint32 i = 0; i < Chunks.size(); i++) {
            if (0 == memcmp(id, Chunks[i].id, 4)) {
                free(Chunks[i].data);
                old_chunk = &Chunks[i];
                break;
            }
        }

        auto chunk = CHUNK();
        memcpy(chunk.id, id, 4);
        chunk.size = size;
        chunk.data = (uint8*)calloc(size, 1);
        memcpy_s(chunk.data, size, data, size);

        if (NULL == old_chunk) {
            Chunks.push_back(chunk);
        } else {
            memcpy(old_chunk, &chunk, sizeof(chunk));
        }

        return chunk.data;
    }

    uint8* RIFF::AddChunk(const char* id, FILE* fp, uint32 size) {
        CHUNK* old_chunk = NULL;
        for (uint32 i = 0; i < Chunks.size(); i++) {
            if (0 == memcmp(id, Chunks[i].id, 4)) {
                free(Chunks[i].data);
                old_chunk = &Chunks[i];
                break;
            }
        }

        auto chunk = CHUNK();
        memcpy(chunk.id, id, 4);
        chunk.size = size;
        chunk.data = (uint8*)calloc(size, 1);
        fread_s(chunk.data, size, size, 1, fp);

        if (NULL == old_chunk) {
            Chunks.push_back(chunk);
        } else {
            memcpy(old_chunk, &chunk, sizeof(chunk));
        }

        return chunk.data;
    }

    void RIFF::RemoveChunk(const char* id) {
        std::vector<RIFF::CHUNK> tmp;
        for (uint32 i = 0; i < Chunks.size(); i++) {
            if (0 == memcmp(id, Chunks[i].id, 4)) {
                free(Chunks[i].data);
            } else {
                tmp.push_back(Chunks[i]);
            }
        }
        Chunks.clear();
        for (uint32 i = 0; i < tmp.size(); i++) {
            Chunks.push_back(tmp[i]);
        }
    }

    void RIFF::Load(wchar_t* path) {
        FILE* fp = NULL;
        _wfopen_s(&fp, path, L"rb");
        if (NULL == fp) {
            return;
        }

        char riff_id[5] = { 0 };
        uint32 riff_size = 0;
        char riff_type[5] = { 0 };
        fread_s(riff_id, 4, 4, 1, fp);
        fread_s(&riff_size, 4, 4, 1, fp);
        fread_s(riff_type, 4, 4, 1, fp);

        if (0 != strcmp("RIFF", riff_id) || !CheckFile(riff_type)) {
            fclose(fp);
            return;
        }

        Load(fp, riff_size - 4);

        fclose(fp);
    }

    void RIFF::Load(FILE* fp, uint32 size) {
        uint32 pos = 0;
        while (pos < size) {
            char chunk_id[5] = { 0 };
            uint32 chunk_size = 0;
            fread_s(chunk_id, 4, 4, 1, fp);
            fread_s(&chunk_size, 4, 4, 1, fp);
            pos += 8;
            pos += chunk_size;

            if (0 == strcmp("LIST", chunk_id)) {
                uint32 list_size = chunk_size - 4;
                char list_id[5] = { 0 };
                fread_s(list_id, 4, 4, 1, fp);

                if (0 == strcmp("INFO", list_id)) {
                    loadInfo(fp, list_size);
                }
                else {
                    LoadList(fp, list_id, list_size);
                }
            }
            else {
                LoadChunk(fp, chunk_id, chunk_size);
            }
        }
    }

    uint32 RIFF::Write(FILE* fp) {
        fpos_t size_pos;
        fwrite(LIST_ID, 4, 1, fp);
        fgetpos(fp, &size_pos);
        fwrite(&ZERO_PAD, 4, 1, fp);
        fwrite(ID(), 4, 1, fp);

        uint32 size = 4;
        size += writeChunk(fp);
        size += writeInfo(fp);

        for (uint32 i = 0; i < List.size(); i++) {
            size += List[i]->Write(fp);
        }

        fpos_t term_pos;
        fgetpos(fp, &term_pos);
        fseek(fp, size_pos, SEEK_SET);
        fwrite(&size, 4, 1, fp);
        fseek(fp, term_pos, SEEK_SET);

        return size + 8;
    }

    bool RIFF::CheckFile(char* type) {
        return true;
    }

    void RIFF::LoadChunk(FILE* fp, char* type, uint32 size) {
        fseek(fp, size, SEEK_CUR);
    }

    void RIFF::LoadInfo(FILE* fp, char* type, uint32 size) {
        fseek(fp, size, SEEK_CUR);
    }

    void RIFF::LoadList(FILE* fp, char* type, uint32 size) {
        fseek(fp, size, SEEK_CUR);
    }

    void RIFF::loadInfo(FILE* fp, uint32 size) {
        char info_type[5] = { 0 };
        uint32 info_size = 0;
        uint32 pos = 0;
        while (pos < size) {
            fread_s(info_type, 4, 4, 1, fp);
            fread_s(&info_size, 4, 4, 1, fp);

            info_size += (0 == info_size % 2) ? 0 : 1;
            pos += info_size + 8;

            LoadInfo(fp, info_type, info_size);
        }
    }

    uint32 RIFF::writeChunk(FILE* fp) {
        uint32 size = 0;
        for (uint32 i = 0; i < Chunks.size(); i++) {
            auto ck = Chunks[i];
            fwrite(ck.id, 4, 1, fp);
            fwrite(&ck.size, 4, 1, fp);
            fwrite(ck.data, ck.size, 1, fp);
            size += ck.size + 8;
        }
        return size;
    }

    uint32 RIFF::writeInfo(FILE* fp) {
        if (0 == Info.size()) {
            return 0;
        }

        fpos_t size_pos;
        fwrite(LIST_ID, 4, 1, fp);
        fgetpos(fp, &size_pos);
        fwrite(&ZERO_PAD, 4, 1, fp);
        fwrite(INFO_ID, 4, 1, fp);

        uint32 size = 4;
        for (uint32 i = 0; i < Info.size(); i++) {
            auto info = Info[i];
            auto pad_size = info.size;
            pad_size += 0 < info.size % 2 ? 1 : 0;
            fwrite(info.id, 4, 1, fp);
            fwrite(&info.size, 4, 1, fp);
            fwrite(info.data, pad_size, 1, fp);
            size += pad_size + 8;
        }

        fpos_t term_pos;
        fgetpos(fp, &term_pos);
        fseek(fp, size_pos, SEEK_SET);
        fwrite(&size, 4, 1, fp);
        fseek(fp, term_pos, SEEK_SET);

        return size + 8;
    }
}
