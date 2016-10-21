#ifndef _MUJINJA_H_
#define _MUJINJA_H_

/* this file is open-source see the LICENSE file for more information */
struct MjTemplate
{
    char *text;
    int type;
    struct MjTemplate *next;
};

struct MjReplacement
{
    char *name;
    char *value;
};

enum
{
    OOM=-1,
    NCP=-2,
    UAC=-3,
    NOT_FOUND=-4
};
enum
{
    TEXT,
    REPLACEMENT
};
struct MjTemplate *mj_parse_template_file(char *filename, int *err);
struct MjTemplate *mj_parse_template(char *start, int *err);
void mj_free_template(struct MjTemplate *tplt);
char *mj_render_template(struct MjTemplate *tplt, struct MjReplacement *replacements, int *err);
char *mj_select_replacement(struct MjReplacement *replacements, char *name, int *err);
#endif
