#include "mujinja.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

/* This file is open-source, see LICENSE for more details */
struct MjTemplate *mj_parse_template_file(char *filename, int *err)
{
    FILE *template_file;
    template_file = fopen(filename, "r");
    if(template_file == NULL)
    {
        *err = NOT_FOUND;
        return NULL;
    }
    fseek(template_file, 0, SEEK_END);
    long size = ftell(template_file);
    rewind(template_file);
    char *start = malloc( size + 1);
    if(start == NULL)
    {
        fclose(template_file);
        *err = OOM;
        return NULL;
    }
    int result = fread(start, 1, size, template_file);
    start[size] = 0;
    fclose(template_file);
    if(result != size)
    {
        *err = NOT_FOUND;
        return NULL;
    }
    struct MjTemplate *tplt = mj_parse_template(start, err);
    free(start);
    return tplt;
}
struct MjTemplate *mj_parse_template(char *start, int *err)
{
    char *last_insert = start;
    char *it = start;
    char *word_end = it;
    struct MjTemplate *tplt_start = NULL;
    struct MjTemplate *tplt_it = NULL;
    for(;*it != 0;it++)
    {
        if(*it == '{')
        {
            if(*(it + 1) == '{')
            {
                if(it > last_insert)
                {
                    /*Check wether there is text before the Insert */
                    if(tplt_start == NULL)
                    {
                        tplt_start = calloc(1, sizeof(struct MjTemplate));
                        if(tplt_start == NULL)
                        {
                            *err = OOM;
                            return NULL;
                        }
                        tplt_it = tplt_start;
                    }
                    else
                    {
                        tplt_it->next = calloc(1, sizeof(struct MjTemplate));
                        if(tplt_it->next == NULL)
                        {
                            *err = OOM;
                            return tplt_start;
                        }
                        tplt_it = tplt_it->next;
                    }
                    
                    tplt_it->text = malloc(it-last_insert + 1);
                    if(tplt_it->text == NULL)
                    {
                        *err = OOM;
                        return tplt_start;
                    }
                    memcpy(tplt_it->text, last_insert, it-last_insert);
                    tplt_it->text[it-last_insert] = 0;
                    tplt_it->type = TEXT;
                }
                /* fast forward through the {*/
                it = it + 2;

                for(;*it != 0 && isspace(*it);it++);

                if(*it == 0)
                {
                    *err = NCP;
                    return tplt_start;
                }
                /* escaped text*/
                if(*it == '\'')
                {
                    it++;
                    char *word_end = it;
                    for(;*word_end != 0 && *word_end != '\'';word_end++);

                    if(*word_end == 0)
                    {
                        *err = NCP;
                        return tplt_start;
                    }
                    if(tplt_start == NULL)
                    {
                        tplt_start = calloc(1, sizeof(struct MjTemplate));
                        if(tplt_start == NULL)
                        {
                            *err = OOM;
                            return NULL;
                        }
                        tplt_it = tplt_it;
                    }
                    else
                    {
                        tplt_it->next = calloc(1, sizeof(struct MjTemplate));
                        if(tplt_it->next == NULL)
                        {
                            *err = OOM;
                            return tplt_start;
                        }
                        tplt_it = tplt_it->next;
                    }
                    tplt_it->type = TEXT;
                    tplt_it->text = malloc(word_end-it + 1);
                    if(tplt_it->text == NULL)
                    {
                        *err = OOM;
                        return tplt_start;
                    }
                    memcpy(tplt_it->text, it, word_end-it);
                    tplt_it->text[word_end-it];
                    it = word_end + 1;

                }
                else
                {
                    char *word_end = it;
                    for(;*word_end != 0 &&
                            !isspace(*word_end) &&
                                *word_end != '}'; word_end++);
                    if(*word_end == 0)
                    {
                        
                        *err = NCP;
                        return tplt_start;
                    }
                    if(tplt_start == NULL)
                    {
                        tplt_start = calloc(1, sizeof(struct MjTemplate));
                        if(tplt_start == NULL)
                        {
                            *err = OOM;
                            return NULL;
                        }
                        tplt_it = tplt_it;
                    }
                    else
                    {
                        tplt_it->next = calloc(1, sizeof(struct MjTemplate));
                        if(tplt_it->next == NULL)
                        {
                            *err = OOM;
                            return tplt_start;
                        }
                        tplt_it = tplt_it->next;
                    }
                    tplt_it->type = REPLACEMENT;
                    tplt_it->text = malloc(word_end - it + 1);
                    if(tplt_it->text == NULL)
                    {
                        *err = OOM;
                        return tplt_start;
                    }
                    memcpy(tplt_it->text, it, (word_end - it));
                    tplt_it->text[word_end - it] = 0;
                    it = word_end;
                }
                for(;*it != 0 && isspace(*it);it++);
                if(*it == 0)
                {
                    
                    *err = NCP;
                    return tplt_start;
                }
                if(*it != '}')
                {
                    *err = UAC;
                    return tplt_start;
                }
                it++;
                if(*it != '}')
                {
                    *err = UAC;
                    return tplt_start;
                }
                last_insert = it + 1;
            }
        }
    }
    if(*last_insert != 0)
    {
        if(tplt_it == NULL)
        {
            tplt_it = malloc(sizeof(struct MjTemplate));
            if(tplt_it == NULL)
            {
                *err = OOM;
                return NULL;
            }
            tplt_start = tplt_it;
        }
        else
        {
            tplt_it->next = malloc(sizeof(struct MjTemplate));
            if(tplt_it->next == NULL)
            {
                *err = OOM;
                return tplt_start;
            }
            tplt_it = tplt_it->next;
        }
        tplt_it->text = malloc( strlen(last_insert) + 1);
        if(tplt_it->text == NULL)
        {
            *err = OOM;
            return tplt_start;
        }
        memcpy(tplt_it->text, last_insert, strlen(last_insert) + 1);

    }
    *err = 0;
    return tplt_start;
}

void mj_free_template(struct MjTemplate *tplt)
{
    if(tplt != NULL)
    {
        for(;tplt != NULL;)
        {
            free(tplt->text);
            struct MjTemplate *temp = tplt->next;
            free(tplt);
            tplt = temp;
        }
    }
}

char *mj_render_template(struct MjTemplate *tplt, struct MjReplacement *replacements, int *err)
{
    char *result;
    struct MjTemplate *tplt_temp = tplt;
    size_t str_size;

    for(;tplt_temp != NULL;tplt_temp = tplt_temp->next)
    {
        if(tplt_temp->type == TEXT)
        {
            str_size += strlen(tplt_temp->text);
        }
        if(tplt_temp->type == REPLACEMENT)
        {
            char *insert = mj_select_replacement(replacements, tplt_temp->text, err);
            if(*err == NOT_FOUND)
            {
                return tplt_temp->text;
            }
            str_size += strlen(insert);
        }
    }
    result = calloc(1, str_size);
    if(result == NULL)
    {
        *err = OOM;
        return NULL;
    }
    int i = 0;
    for(tplt_temp = tplt; tplt_temp != NULL; tplt_temp = tplt_temp->next)
    {
        if(tplt_temp->type == TEXT)
        {
            memcpy(result+i, tplt_temp->text, strlen(tplt_temp->text));
            i += strlen(tplt_temp->text);
        }
        else if(tplt_temp->type == REPLACEMENT)
        {
            char *insert = mj_select_replacement(replacements, tplt_temp->text, err);
            memcpy(result+i, insert, strlen(insert));
            i += strlen(insert);
        }
    }
    return result;
}

char *mj_select_replacement(struct MjReplacement *replacements, char *name, int *err)
{
    int i = 0;
    for(;replacements[i].name != NULL && replacements[i].value != NULL;i++)
    {
        if(strcmp(replacements[i].name, name) == 0)
        {
            break;
        }
    }
    if(replacements[i].name == NULL && replacements[i].value == NULL)
    {
        *err = NOT_FOUND;
        return NULL;
    }
    return replacements[i].value;
}
