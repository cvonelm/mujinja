#include "../mujinja.h"
#include <stdio.h>
#include <stdlib.h>
/* This program is open source see the LICENSE file in the root directory of this project for more
 * information
 *
 * Compiling:
 * cc -o parsing_example parsing_example.c ../mujinja.c
 */
int main()
{
    /* for error checking */
    int err;
    /* let the application compile the template */
    /* things like {{ can be escaped like this: {{ '{{' }}
     */
    struct MjTemplate *test = mj_parse_template("Hello {{ '{{ bla }}' }} {{ person.first_name }} {{person.last_name }}", &err);
    /* Out Of Memory */
    if(err == OOM)
    {
        printf("No memory could be allocated\n");
        return -1;
    }
    /* No close paren */
    else if(err == NCP)
    {
        printf("Missing close paren\n");
        return -1;
    }
    /* Unallowed character */
    else if(err == UAC)
    {
        printf("Invalid character, expected '}'");
        return -1;
    }
    /* the replacements argument is an array of MjReplacement, which have the structure {name, value }
     * the last element must be { NULL, NULL } ! */
    struct MjReplacement replacements[]=
    {{ "person.first_name", "Hans" },
     { "person.last_name", "Peter" },
     { NULL, NULL}};
    /* let it render the template */
    char * rendered = mj_render_template(test, replacements, &err);
    /* some replacement wasnt found in the MjReplacement array */
    if(err == NOT_FOUND)
    {
        /* the name of the missing replacement is returned by mj_render_template then */
        printf("missing replacement: %s\n", rendered);
        return -1;
    }
    else if(err == OOM)
    {
        printf("No memory could be allocated\n");
        return -1;
    }
    printf("%s\n", rendered);
    /* the string returned by mj_render_template must be freed */
    free(rendered);
    /* free the template */
    mj_free_template(test);
    return 0;
}
