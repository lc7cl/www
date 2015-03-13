#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct
{
    ngx_str_t dns_resolve;
    ngx_str_t cfg_str;
} ngx_http_dns_t;

//static char* ngx_http_dns_resolve(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_read_cfg(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void * ngx_http_create_srv_conf(ngx_conf_t *cf);
static ngx_int_t ngx_http_dns_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_dns_init(ngx_conf_t *cf);

static ngx_command_t ngx_http_dns_commands[] = {

    {
        ngx_string("dns_resolve"),
        NGX_HTTP_LOC_CONF,
        //ngx_http_dns_resolve,
        NULL,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_dns_t, dns_resolve),
        NULL },
    {
        ngx_string("my_test"),
        NGX_HTTP_SRV_CONF,
        ngx_http_read_cfg,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_dns_t, cfg_str),
        NULL },

    ngx_null_command
};

static ngx_http_module_t ngx_http_dns_module_ctx = {
    NULL,
    ngx_http_dns_init,

    NULL,
    NULL,

    NULL,
    NULL,

    ngx_http_create_srv_conf,
    NULL,
};

ngx_module_t ngx_http_dns_module = {
    NGX_MODULE_V1,
    &ngx_http_dns_module_ctx,
    ngx_http_dns_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

#if 0
static char* ngx_http_dns_resolve(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    return  NULL;
}
#endif

static char* ngx_http_read_cfg(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "########%s", "");
    if (ngx_conf_set_str_slot(cf, cmd, conf) == (char*)NGX_ERROR)
        return (char*)NGX_ERROR;
    
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "########%s", "");

    return (char *)NGX_OK;
}

static void * ngx_http_create_srv_conf(ngx_conf_t *cf)
{
    ngx_http_dns_t *srv_conf;

    srv_conf = ngx_pcalloc(cf->pool, sizeof(*srv_conf));
    if (srv_conf == NULL) {
        return NULL;
    }

    ngx_str_null(&srv_conf->cfg_str);

    return srv_conf;
}

static ngx_int_t ngx_http_dns_handler(ngx_http_request_t *r)
{
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "11111111111");
    return NGX_OK;
}

static ngx_int_t ngx_http_dns_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;

    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "%s %d", __func__, __LINE__);
    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
    if ( h == NULL ) 
        return NGX_ERROR;
    *h = ngx_http_dns_handler;
    return NGX_OK;
}
