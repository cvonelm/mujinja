#mujinja -- Documentation

##Introduction
mujinja is a minimal implementation of a templating engine, roughly following the style
of jinja2 templates. It supports only the replacement of text.

##Template syntax
mujinja templates can be any type of  text containing replacements.

Replacements look like this:

    {{ variable.name }}

Which will be replaced when rendering the template with the value of 'variable.name'.
The variable name may contain any character as long as it doesnt start with an ' or 
contains whitespace characters. It is advised to keep the variable names simple.

strings can be escaped in this manner:
    
    {{ '{{ escaped }}' }}
the text in the quotes will be rendered as
    
    {{ escaped }}

##Using the library

a full example can be found in  examples/parsing\_example.c
for parsing templates from files see examples/parsing\_file\_example.c

#Public API

##Structures

###struct MjReplacement
A structure MjReplacement describes which variable is to be replaced with
which text 

Members:

- char \*name | the name of the variable to be replaced
- char \*value | the value that will replace the variable

##Functions

###struct MjTemplate \*mj\_parse\_template(char \*start, int \*err)

Arguments:

- char \*start | the text that will be parsed
- int \*err | a pointer to an int which is filled with an error code
Returns:

a pointer to a struct MjTemplate holding the Template

###struct MjTemplate \*mj\_parse\_template\_file(char \*filename, int \*err)
Arguments:
- char \* filename | the filename that will be parsed
- int \*err | a pointer to an int which is filled with an eror code

Returns:
a pointer to a struct MjTemplate holding the Template

###char \*mj\_render\_template(struct MjTemplate \*tplt, struct MjReplacement \*replacements, int \*err)

Arguments:

- struct MjTemplate \*tplt | the template to be rendered
- struct MjReplacement \*replacements | the replacements to be inserted
- int \*err | a pointer to an int which is filled with an error code on error

Returns:
The rendered template. The user is required to free() the string

###void mj\_free\_template(struct MjTemplate \*tplt)

Arguments:

- struct MjTemplate \*tplt | The template to be freed

