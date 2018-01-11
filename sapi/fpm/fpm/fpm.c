
	/* $Id: fpm.c,v 1.23 2008/07/20 16:38:31 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <stdlib.h> /* for exit */

#include "fpm.h"
#include "fpm_children.h"
#include "fpm_signals.h"
#include "fpm_env.h"
#include "fpm_events.h"
#include "fpm_cleanup.h"
#include "fpm_php.h"
#include "fpm_sockets.h"
#include "fpm_unix.h"
#include "fpm_process_ctl.h"
#include "fpm_conf.h"
#include "fpm_worker_pool.h"
#include "fpm_scoreboard.h"
#include "fpm_stdio.h"
#include "fpm_log.h"
#include "zlog.h"

struct fpm_globals_s fpm_globals = {
	.parent_pid = 0,
	.argc = 0,
	.argv = NULL,
	.config = NULL,
	.prefix = NULL,
	.pid = NULL,
	.running_children = 0,
	.error_log_fd = 0,
	.log_level = 0,
	.listening_socket = 0,
	.max_requests = 0,
	.is_child = 0,
	.test_successful = 0,
	.heartbeat = 0,
	.run_as_root = 0,
	.force_stderr = 0,
	.send_config_pipe = {0, 0},
};

int fpm_init(int argc, char **argv, char *config, char *prefix, char *pid, int test_conf, int run_as_root, int force_daemon, int force_stderr) /* {{{ */
{
	fpm_globals.argc = argc;
	fpm_globals.argv = argv;
	if (config && *config) {
		fpm_globals.config = strdup(config);
	}
	fpm_globals.prefix = prefix;
	fpm_globals.pid = pid;
	fpm_globals.run_as_root = run_as_root;
	fpm_globals.force_stderr = force_stderr;

	/**
	 * fpm_config_init_main 解析 php-fpm.conf 配置文件，为每个 worker pool 分配一个 fpm_worker_pool_s 结构。每个 worker pool 的配置在解析后保存到 fpm_worker_pool_s->config 中
	 * fpm_scoreboard_init_main 分配用户记录 worker 进程运行信息的结构，此结构分配在共享内存上
	 * fpm_signals_init_main 创建一个管道，这个管道并不是用来进行 master 进程和 worker 的通信，它只在 master 进程中使用。同时设置 master 的信号处理函数为 sig_handler(),当 master 收到 SIGTERM,SIGINT,SIGUSR1,SIGSUSR2,SIGCHLD,SIGQUIT 这些信号的时候，将调用 sig_handler 处理，
	 * fpm_sockets_init_main 创建每个 worker pool 的套接字，启动后 worker 将监听此 socket 接收请求
	 * fpm_event_init_main 启动 master 的事件管理，fpm 实现了一个事件管理调度用于管理 i/o,定时事件，其中 i/o 事件根据不同平台选择 kqueue,epoll,poll,select 来管理。定时事件就是定时器，一定时间后触发某个事件
	 * fpm_init()中主要的就是上面介绍的几个 init 过程，在完成这些初始化之后，最关键的就是 fpm_run 的操作。此环节将 fork 子进程，启动进程管理器，执行后 master 进程将不会返回这个函数，只有各自的 worker 进程会返回，也就是说 main 函数中调用 fpm_run 之后的操作都是 worker 进程进行的
	 */
	if (0 > fpm_php_init_main()           ||
	    0 > fpm_stdio_init_main()         ||
	    0 > fpm_conf_init_main(test_conf, force_daemon) ||
	    0 > fpm_unix_init_main()          ||
	    0 > fpm_scoreboard_init_main()    ||
	    0 > fpm_pctl_init_main()          ||
	    0 > fpm_env_init_main()           ||
	    0 > fpm_signals_init_main()       ||
	    0 > fpm_children_init_main()      ||
	    0 > fpm_sockets_init_main()       ||
	    0 > fpm_worker_pool_init_main()   ||
	    0 > fpm_event_init_main()) {

		if (fpm_globals.test_successful) {
			exit(FPM_EXIT_OK);
		} else {
			zlog(ZLOG_ERROR, "FPM initialization failed");
			return -1;
		}
	}

	if (0 > fpm_conf_write_pid()) {
		zlog(ZLOG_ERROR, "FPM initialization failed");
		return -1;
	}

	fpm_stdio_init_final();
	zlog(ZLOG_NOTICE, "fpm is running, pid %d", (int) fpm_globals.parent_pid);

	return 0;
}
/* }}} */

/*	children: return listening socket
	parent: never return */
int fpm_run(int *max_requests) /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	// 编译 worker pool
	/* create initial children in all pools */
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		int is_parent;

		// 调用 fpm_children_make() fork 子进程
		is_parent = fpm_children_create_initial(wp);

		if (!is_parent) {
			// fork 出的 worker 进程
			goto run_child;
		}

		/* handle error */
		if (is_parent == 2) {
			fpm_pctl(FPM_PCTL_STATE_TERMINATING, FPM_PCTL_ACTION_SET);
			fpm_event_loop(1);
		}
	}

	// master 进程将进入 event 循环，不再往下走
	/* run event loop forever */
	fpm_event_loop(0);

	// 只有 worker 进程将会进入到这里
run_child: /* only workers reach this point */

	fpm_cleanups_run(FPM_CLEANUP_CHILD);

	*max_requests = fpm_globals.max_requests;
	// 返回监听的套接字
	return fpm_globals.listening_socket;
}
/* }}} */

