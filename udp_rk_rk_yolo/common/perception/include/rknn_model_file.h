#ifndef RKNN_MODEL_FILE_H
#define RKNN_MODEL_FILE_H

#include <stdio.h>
#include <stdlib.h>

// Read the model from an already-open file, which this function always closes.
static inline void *read_rknn_model(FILE *fp, size_t *model_size, int diagnostics)
{
    fseek(fp, 0, SEEK_END);
    *model_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    void *model_data = malloc(*model_size);
    if (!model_data)
    {
        if (diagnostics)
            printf("分配模型内存失败\n");
        fclose(fp);
        return NULL;
    }

    if (fread(model_data, 1, *model_size, fp) != *model_size)
    {
        if (diagnostics)
            printf("读取模型文件失败\n");
        free(model_data);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return model_data;
}

#endif
