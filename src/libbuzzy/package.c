/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>

#include <libcork/core.h>
#include <libcork/os.h>
#include <libcork/helpers/errors.h>

#include "buzzy/action.h"
#include "buzzy/callbacks.h"
#include "buzzy/error.h"
#include "buzzy/package.h"
#include "buzzy/version.h"


#if !defined(BZ_DEBUG_PACKAGES)
#define BZ_DEBUG_PACKAGES  0
#endif

#if BZ_DEBUG_PACKAGES
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


/*-----------------------------------------------------------------------
 * Packages
 */

struct bz_package {
    const char  *name;
    struct bz_version  *version;
    struct bz_dependency  *dep;
    void  *user_data;
    bz_user_data_free_f  user_data_free;
    bz_pdb_install_f  install;
    struct bz_action  *install_action;
};


struct bz_package *
bz_package_new(const char *name, struct bz_version *version,
               struct bz_dependency *dep,
               void *user_data, bz_user_data_free_f user_data_free,
               bz_pdb_install_f install)
{
    struct bz_package  *package = cork_new(struct bz_package);
    package->name = cork_strdup(name);
    package->version = version;
    package->dep = dep;
    package->user_data = user_data;
    package->user_data_free = user_data_free;
    package->install = install;
    package->install_action = NULL;
    return package;
}

void
bz_package_free(struct bz_package *package)
{
    cork_strfree(package->name);
    bz_version_free(package->version);
    if (package->user_data_free != NULL) {
        package->user_data_free(package->user_data);
    }
    if (package->install_action != NULL) {
        bz_action_free(package->install_action);
    }
    free(package);
}

struct bz_action *
bz_package_install_action(struct bz_package *package)
{
    if (package->install_action == NULL) {
        package->install_action = package->install(package->user_data);
    }
    return package->install_action;
}


/*-----------------------------------------------------------------------
 * Package databases
 */

struct bz_pdb {
    const char  *name;
    void  *user_data;
    bz_user_data_free_f  user_data_free;
    bz_pdb_satisfy_f  satisfy;
    struct cork_dllist_item  item;
};


struct bz_pdb *
bz_pdb_new(const char *name,
           void *user_data, bz_user_data_free_f user_data_free,
           bz_pdb_satisfy_f satisfy)
{
    struct bz_pdb  *pdb = cork_new(struct bz_pdb);
    pdb->name = cork_strdup(name);
    pdb->user_data = user_data;
    pdb->user_data_free = user_data_free;
    pdb->satisfy = satisfy;
    return pdb;
}

void
bz_pdb_free(struct bz_pdb *pdb)
{
    cork_strfree(pdb->name);
    if (pdb->user_data_free != NULL) {
        pdb->user_data_free(pdb->user_data);
    }
    free(pdb);
}

struct bz_package *
bz_pdb_satisfy_dependency(struct bz_pdb *pdb, struct bz_dependency *dep)
{
    return pdb->satisfy(pdb->user_data, dep);
}


/*-----------------------------------------------------------------------
 * Package database registry
 */

static struct cork_dllist  pdbs;

static void
free_pdb(struct cork_dllist_item *item, void *user_data)
{
    struct bz_pdb  *pdb = cork_container_of(item, struct bz_pdb, item);
    bz_pdb_free(pdb);
}

static void
free_pdbs(void)
{
    cork_dllist_map(&pdbs, free_pdb, NULL);
}

CORK_INITIALIZER(init_pdbs)
{
    cork_dllist_init(&pdbs);
    cork_cleanup_at_exit(0, free_pdbs);
}

void
bz_pdb_register(struct bz_pdb *pdb)
{
    cork_dllist_add(&pdbs, &pdb->item);
}

struct bz_package *
bz_satisfy_dependency(struct bz_dependency *dep)
{
    struct cork_dllist_item  *curr;
    for (curr = cork_dllist_start(&pdbs); !cork_dllist_is_end(&pdbs, curr);
         curr = curr->next) {
        struct bz_pdb  *pdb = cork_container_of(curr, struct bz_pdb, item);
        struct bz_package  *package = bz_pdb_satisfy_dependency(pdb, dep);
        if (CORK_UNLIKELY(cork_error_occurred())) {
            return NULL;
        } else if (package != NULL) {
            return package;
        }
    }

    bz_cannot_satisfy
        ("Cannot satisfy dependency %s", bz_dependency_to_string(dep));
    return NULL;
}
