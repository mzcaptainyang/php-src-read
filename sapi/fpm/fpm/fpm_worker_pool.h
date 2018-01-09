
	/* $Id: fpm_worker_pool.h,v 1.13 2008/08/26 15:09:15 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_WORKER_POOL_H
#define FPM_WORKER_POOL_H 1

#include "fpm_conf.h"
#include "fpm_shm.h"

struct fpm_worker_pool_s;
struct fpm_child_s;
struct fpm_child_stat_s;
struct fpm_shm_s;

enum fpm_address_domain {
	FPM_AF_UNIX = 1,
	FPM_AF_INET = 2
};

	/**
	 * worker pool 的结构体
	 */
struct fpm_worker_pool_s {
	// 指向下一个 worker pool
	struct fpm_worker_pool_s *next;
	// php-fpm.conf 的配置，：pm,max_children,start_servers
	struct fpm_worker_pool_config_s *config;
	char *user, *home;									/* for setting env USER and HOME */
	enum fpm_address_domain listen_address_domain;
	// 监听的套接字
	int listening_socket;
	int set_uid, set_gid;								/* config uid and gid */
	int socket_uid, socket_gid, socket_mode;

	/* runtime */
	// 当前 pool 的 worker 链表，每个 worker 对应一个 fpm_child_s 结构
	struct fpm_child_s *children;
	int running_children;
	int idle_spawn_rate;
	int warn_max_children;
#if 0
	int warn_lq;
#endif
        // 记录 worker 的运行信息，比如空闲，忙碌的 worker 数等
	struct fpm_scoreboard_s *scoreboard;
	int log_fd;
	char **limit_extensions;

	/* for ondemand PM */
	struct fpm_event_s *ondemand_event;
	int socket_event_set;

#ifdef HAVE_FPM_ACL
	void *socket_acl;
#endif
};

struct fpm_worker_pool_s *fpm_worker_pool_alloc();
void fpm_worker_pool_free(struct fpm_worker_pool_s *wp);
int fpm_worker_pool_init_main();

extern struct fpm_worker_pool_s *fpm_worker_all_pools;

#endif

