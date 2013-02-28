
/*
 * Copyright (C) Kornel Schadl
 */

#include <ngx_core.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>

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
    size_t n,w;
    char buff[1024],cwd[MAXPATHLEN],*nwd,*p;
    FILE *in,*out;
    int e=1,h;

    p=getcwd(cwd,sizeof(cwd));
    if(!p){
        ngx_conf_log_error(NGX_LOG_EMERG,cf,errno,"getcwd() failed - errno %d",errno);
        return NGX_CONF_ERROR;
    }

    nwd=strndup((char *)cf->conf_file->file.name.data,(size_t)cf->conf_file->file.name.len);
    p=strrchr(nwd,'/');
    if(p) *p=0;

    h=chdir(nwd);
    free(nwd);
    if(h<0){
        ngx_conf_log_error(NGX_LOG_EMERG,cf,errno,"chdir() failed - errno %d",errno);
        return NGX_CONF_ERROR;
    }

    in=popen((char *)v[2].data,"r");
    if(!in){
        ngx_conf_log_error(NGX_LOG_EMERG,cf,errno,"popen() \"%s\" failed",v[2].data);
        goto end;
    }

    out=fopen((char *)v[1].data,"w");
    if(!out){
        ngx_conf_log_error(NGX_LOG_EMERG,cf,errno,"fopen() \"%s\" failed",v[1].data);
        goto end;
    }

    while(1){
        n=fread(buff,1,sizeof(buff),in);
        if(n<sizeof(buff) && ferror(in)){
            ngx_conf_log_error(NGX_LOG_EMERG,cf,errno,"fread() \"%s\" failed",v[2].data);
            goto end;
        }
        if(n>0){
            w=fwrite(buff,1,n,out);
            if(w<n && ferror(out)){
                ngx_conf_log_error(NGX_LOG_EMERG,cf,errno,"fwrite() \"%s\" failed",v[1].data);
                goto end;
            }
        }

        if(feof(in)) break;
    }
    e=0;

end:
    if(out) fclose(out);
    if(in) pclose(in);
    if(chdir(cwd)<0 || e) return NGX_CONF_ERROR;

    return ngx_conf_parse(cf,&v[1]);
}
