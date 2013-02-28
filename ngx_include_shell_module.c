
/*
 * Copyright (C) Kornel Schadl
 */

#include <ngx_core.h>
#include <stdio.h>

char *ngx_include_shell(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t ngx_include_shell_commands[] = {
    { ngx_string("include_shell"),
      NGX_ANY_CONF|NGX_CONF_TAKE2,
      ngx_include_shell,
      0,
      0,
      NULL },

      ngx_null_command
};

ngx_module_t ngx_include_shell_module = {
    NGX_MODULE_V1,
    NULL,
    ngx_include_shell_commands,
    NGX_CONF_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

char *
ngx_include_shell(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_str_t *v=cf->args->elts;
    size_t n;
    char buff[1024];

    FILE *in=popen((char *)v[2].data,"r");
    FILE *out=fopen((char *)v[1].data,"w");
    while(1){
        n=fread(buff,1,sizeof(buff),in);
        if(n>0) fwrite(buff,1,n,out);
        if(feof(in)) break;
    }
    fclose(out);
    pclose(in);

    return ngx_conf_parse(cf,&v[1]);
}
