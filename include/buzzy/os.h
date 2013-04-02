/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef BUZZY_OS_H
#define BUZZY_OS_H

#include <stdarg.h>

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/os.h>


/*-----------------------------------------------------------------------
 * Executing subprocesses
 */


/* Execute the subprocess, wait for it to finish, and capture its stdout and
 * stderr streams into the given buffers.  (The buffers can be NULL, in which
 * case that output stream is ignored.) */

int
bz_subprocess_v_get_output(struct cork_buffer *out, struct cork_buffer *err,
                           bool *successful, va_list args);

CORK_ATTR_SENTINEL
int
bz_subprocess_get_output(struct cork_buffer *out, struct cork_buffer *err,
                         bool *successful, ...);

int
bz_subprocess_get_output_exec(struct cork_buffer *out, struct cork_buffer *err,
                              bool *successful, struct cork_exec *exec);


/* Execute the subprocess and wait for it to finish.
 *
 * If verbose is true, then the subprocess's stdout and stderr will be the same
 * as the calling process, so the end user will see the subprocess's output.  If
 * verbose is false, then the subprocess's output will be ignored.
 *
 * If successful is non-NULL, we'll fill it in with whether the subprocess's
 * exit code was 0 (indicating success). */

int
bz_subprocess_v_run(bool verbose, bool *successful, va_list args);

CORK_ATTR_SENTINEL
int
bz_subprocess_run(bool verbose, bool *successful, ...);

int
bz_subprocess_run_exec(bool verbose, bool *successful, struct cork_exec *exec);


/*-----------------------------------------------------------------------
 * Creating files and directories
 */

struct cork_file *
bz_create_file(struct cork_path *path, struct cork_buffer *src);

int
bz_create_file_from_string(const char *path, struct cork_buffer *src);

/* Takes control of path.  You must delete the directory with
 * bz_delete_directory. */
struct cork_file *
bz_create_directory(struct cork_path *path);

int
bz_create_directory_from_string(const char *path);

int
bz_file_exists(struct cork_path *path, bool *exists);

int
bz_file_exists_from_string(const char *path, bool *exists);


#endif /* BUZZY_OS_H */
