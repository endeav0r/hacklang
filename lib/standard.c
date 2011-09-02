#include "standard.h"

void lib_standard_register (struct in_s * in)
{
    capi_register_function(in, lib_standard_fread,  "fread");
    capi_register_function(in, lib_standard_print,  "print");
    capi_register_function(in, lib_standard_str,    "str");
    capi_register_function(in, lib_standard_strlen, "strlen");
    capi_register_function(in, lib_standard_substr, "substr");
}


int lib_standard_str (struct capi_s * capi)
{
    struct var_s * var;
    
    if (capi_size(capi) == 0) {
        fprintf(stderr, "LIB_STANDARD_STR: NOARG\n");
        exit(-1);
    }
    else if (capi_size(capi) == 1) {
        var = var_create(TYPE_STRING, capi_to_string(capi, 0));
        capi_pop(capi);
        capi_push(capi, var);
        return 1;
    }
    else {
        fprintf(stderr, "LIB_STANDARD_STR: TOO MANY ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_standard_strlen (struct capi_s * capi)
{
    char * string;
    struct var_s * var;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) == CAPI_TYPE_STRING)
            string = capi_to_string(capi, 0);
        else {
            fprintf(stderr, "LIB_STANDARD_STRLEN: ARG MUST BE STRING (%d)\n",
                    capi_type(capi, 0));
            exit(-1);
        }
        
        var = var_create(TYPE_INT, NULL);
        var->i = strlen(string);
        
        capi_pop(capi);
        capi_push(capi, var);
        
        return 1;
    }
    else {
        fprintf(stderr, "LIB_STANDARD_PRINT: TAKES ONE ARGUMENT (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_standard_substr (struct capi_s * capi)
{
    char * string;
    char * new_string;
    int start;
    int end;
    int len;
    struct var_s * var;
    if (capi_size(capi) == 3) {
        if (capi_type(capi, 0) == CAPI_TYPE_STRING)
            string = capi_to_string(capi, 0);
        else {
            fprintf(stderr, "LIB_STANDARD_SUBSTR: FIRST ARG MUST BE STRING (%d)\n",
                    capi_type(capi, 0));
            exit(-1);
        }
        if (capi_type(capi, 1) == CAPI_TYPE_INT)
            start = capi_to_int(capi, 1);
        else {
            fprintf(stderr, "LIB_STANDARD_SUBSTR: SECOND ARG MUST BE INT\n");
            exit(-1);
        }
        if (capi_type(capi, 2) == CAPI_TYPE_INT)
            end = capi_to_int(capi, 2);
        else {
            fprintf(stderr, "LIB_STANDARD_SUBSTR: THIRD ARG MUST BE INT\n");
            exit(-1);
        }
        
        len = strlen(string);
        
        if (start < 0)
            start = len + start;
        if (start < 0) {
            fprintf(stderr,
                    "LIB_STANDARD_SUBSTR: START IS BEFORE BEGINNING OF STRING\n");
            exit(-1);
        }
        if (start >= len) {
            fprintf(stderr, "LIB_STANDARD_SUBSTR: START GREATER THAN STRLEN\n");
            exit(-1);
        }
        
        if (end < 0)
            end = len + end;
        if (end < 0) {
            fprintf(stderr,
                    "LIB_STANDARD_SUBSTR: END IS BEFORE BEGINNING OF STRING\n");
            exit(-1);
        }
        if (end >= len) {
            fprintf(stderr, "LIB_STANDARD_SUBSTR: END IS BEYOND STRLEN\n");
            exit(-1);
        }
        if (end < start) {
            fprintf(stderr, "LIB_STANDARD_SUBSTR: END CANNOT COME BEFORE START\n");
            exit(-1);
        }
        
        new_string = (char *) malloc(end - start + 2);
        strncpy(new_string, &(string[start]), end - start + 1);
        new_string[end - start + 1] = (char) 0x00;
        
        var = var_create(TYPE_STRING, new_string);
        
        capi_pop(capi);
        capi_push(capi, var);
        
        return 1;
    }
    else {
        fprintf(stderr, "LIB_STANDARD_SUBSTR: SUBSTR TAKES 3 ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_standard_print (struct capi_s * capi)
{
    if (capi_size(capi) == 0) {
        fprintf(stderr, "LIB_STANDARD_PRINT: NOARG\n");
        exit(-1);
    }
    else if (capi_size(capi) == 1) {
        printf("%s", capi_to_string(capi, 0));
        fflush(stdout);
        capi_pop(capi);
    }
    else {
        fprintf(stderr, "LIB_STANDARD_PRINT: TOO MANY ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_standard_fread (struct capi_s * capi)
{
    int filesize;
    int bytes_read;
    FILE * fh;
    char * buf;
    char * filename;
    struct var_s * var;

    if (capi_size(capi) != 1) {
        fprintf(stderr, "LIB_STANDARD_FREAD: NUM ARGS != 1\n");
        exit(-1);
    }
    
    filename = capi_to_string(capi, 0);
    
    fh = fopen(filename, "rb");
    if (fh == NULL) {
        fprintf(stderr, "LIB_STANDARD_FREAD: FAILED OPENING %s\n", filename);
        exit(-1);
    }
    
    fseek(fh, 0, SEEK_END);
    filesize = (int) ftell(fh);
    fseek(fh, 0, SEEK_SET);
    
    buf = (char *) malloc(filesize + 1);
    
    
    bytes_read = fread(buf, 1, filesize, fh);
    if (bytes_read != filesize) {
        fprintf(stderr, "LIB_STANDARD_FREAD: READ ERROR FILE %s, %d %d\n",
                filename, filesize, bytes_read);
        exit(-1);
    }
    
    fclose(fh);
    
    buf[filesize] = (char) 0x00;
    
    var = var_create(TYPE_STRING, buf);
    
    capi_pop(capi);
    capi_push(capi, var);
    
    return 1;
}