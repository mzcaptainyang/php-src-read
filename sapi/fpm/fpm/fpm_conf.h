
	/* $Id: fpm_conf.h,v 1.12.2.2 2008/12/13 03:46:49 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CONF_H
#define FPM_CONF_H 1

#include <stdint.h>
#include "php.h"

#define PM2STR(a) (a == PM_STYLE_STATIC ? "static" : (a == PM_STYLE_DYNAMIC ? "dynamic" : "ondemand"))

#define FPM_CONF_MAX_PONG_LENGTH 64

struct key_value_s;

struct key_value_s {
	struct key_value_s *next;
	char *key;
	char *value;
};

/*
 * Please keep the same order as in fpm_conf.c and in php-fpm.conf.in
 */
struct fpm_global_config_s {
	char *pid_file;
	char *error_log;
#ifdef HAVE_SYSLOG_H
	char *syslog_ident;
	int syslog_facility;
#endif
	int log_level;
	int emergency_restart_threshold;
	int emergency_restart_interval;
	int process_control_timeout;
	int process_max;
	int process_priority;
	int daemonize;
	int rlimit_files;
	int rlimit_core;
	char *events_mechanism;
#ifdef HAVE_SYSTEMD
	int systemd_watchdog;
	int systemd_interval;
#endif
};

extern struct fpm_global_config_s fpm_global_config;

/*
 * Please keep the same order as in fpm_conf.c and in php-fpm.conf.in
 */
struct fpm_worker_pool_config_s {
	// pool 的名称，也就是配置 [pool name]
	char *name;
	char *prefix;
	// fpm 的启动用户，配置：user
	char *user;
	// fpm 的启动用户组，配置:group
	char *group;
	// 监听的地址，配置：listen
	char *listen_address;
	// 同时监听的最大 fd 数,配置：listen.backlog
	int listen_backlog;
	/* Using chown */
	char *listen_owner;
	char *listen_group;
	char *listen_mode;
	char *listen_allowed_clients;
	int process_priority;
	// 进程模型,process model:static, dynamic,ondemand
	int pm;
	// 最大 worker 进程数,配置：pm.max_children
	int pm_max_children;
	// 启动时初始化的 worker 进程数,配置：pm.start_servers
	int pm_start_servers;
	// 最小空闲 worker 数，配置：pm.min_spare_servers
	int pm_min_spare_servers;
	// 最大空闲 worker 数，配置：pm.max_spare_servers
	int pm_max_spare_servers;
	// worker 的空闲时间，配置：pm.process_idle_timeout = 10s
	int pm_process_idle_timeout;
	// worker 处理的最多的请求，超过这个值 worker 将被 kill 掉，配置：pm.max_requests = 2048 TODO:弄清楚这个被 kill 掉的原因
	int pm_max_requests;
	char *pm_status_path;
	char *ping_path;
	char *ping_response;
	char *access_log;
	char *access_format;
	char *slowlog;
	int request_slowlog_timeout;
	int request_terminate_timeout;
	int rlimit_files;
	int rlimit_core;
	char *chroot;
	char *chdir;
	int catch_workers_output;
	int clear_env;
	char *security_limit_extensions;
	struct key_value_s *env;
	struct key_value_s *php_admin_values;
	struct key_value_s *php_values;
#ifdef HAVE_APPARMOR
	char *apparmor_hat;
#endif
#ifdef HAVE_FPM_ACL
	/* Using Posix ACL */
	char *listen_acl_users;
	char *listen_acl_groups;
#endif
};

struct ini_value_parser_s {
	char *name;
	char *(*parser)(zval *, void **, intptr_t);
	intptr_t offset;
};

enum {
	PM_STYLE_STATIC = 1,
	PM_STYLE_DYNAMIC = 2,
	PM_STYLE_ONDEMAND = 3
};

int fpm_conf_init_main(int test_conf, int force_daemon);
int fpm_worker_pool_config_free(struct fpm_worker_pool_config_s *wpc);
int fpm_conf_write_pid();
int fpm_conf_unlink_pid();

#endif

