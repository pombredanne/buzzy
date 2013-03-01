/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef BUZZY_PACKAGE_H
#define BUZZY_PACKAGE_H

#include <libcork/core.h>

#include "buzzy/action.h"
#include "buzzy/callbacks.h"
#include "buzzy/version.h"


/*-----------------------------------------------------------------------
 * Packages
 */

struct bz_package;

typedef struct bz_action *
(*bz_pdb_install_f)(void *user_data);


/* Takes control of version, but not dep */
struct bz_package *
bz_package_new(const char *name, struct bz_version *version,
               struct bz_dependency *dep,
               void *user_data, bz_user_data_free_f user_data_free,
               bz_pdb_install_f install);

void
bz_package_free(struct bz_package *package);

/* The package is responsible for freeing the action. */
struct bz_action *
bz_package_install_action(struct bz_package *package);


/*-----------------------------------------------------------------------
 * Package databases
 */

struct bz_pdb;

/* The pdb is responsible for freeing any packages it creates. */
typedef struct bz_package *
(*bz_pdb_satisfy_f)(void *user_data, struct bz_dependency *dep);


struct bz_pdb *
bz_pdb_new(const char *pdb_name,
           void *user_data, bz_user_data_free_f user_data_free,
           bz_pdb_satisfy_f satisfy);

void
bz_pdb_free(struct bz_pdb *pdb);

struct bz_package *
bz_pdb_satisfy_dependency(struct bz_pdb *pdb, struct bz_dependency *dep);


/*-----------------------------------------------------------------------
 * Package database registry
 */

/* Takes control of pdb */
void
bz_pdb_register(struct bz_pdb *pdb);

struct bz_package *
bz_satisfy_dependency(struct bz_dependency *dep);


#endif /* BUZZY_PACKAGE_H */
