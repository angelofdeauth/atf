/*
 * Automated Testing Framework (atf)
 *
 * Copyright (c) 2007, 2008, 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#include <atf-c.h>

#include "atf-c/config.h"
#include "atf-c/env.h"

#include "h_macros.h"

static const char *test_value = "env-value";

static struct varnames {
    const char *lc;
    const char *uc;
} all_vars[] = {
    { "atf_arch",       "ATF_ARCH" },
    { "atf_confdir",    "ATF_CONFDIR" },
    { "atf_libexecdir", "ATF_LIBEXECDIR" },
    { "atf_machine",    "ATF_MACHINE" },
    { "atf_pkgdatadir", "ATF_PKGDATADIR" },
    { "atf_shell",      "ATF_SHELL" },
    { "atf_workdir",    "ATF_WORKDIR" },
    { NULL, NULL }
};

/* ---------------------------------------------------------------------
 * Auxiliary functions.
 * --------------------------------------------------------------------- */

void __atf_config_reinit(void);

static
void
unset_all(void)
{
    const struct varnames *v;
    for (v = all_vars; v->lc != NULL; v++)
        RE(atf_env_unset(v->uc));
}

static
void
compare_one(const char *var, const char *expvalue)
{
    const struct varnames *v;

    printf("Checking that %s is set to %s\n", var, expvalue);

    for (v = all_vars; v->lc != NULL; v++) {
        if (strcmp(v->lc, var) == 0)
            ATF_CHECK_STREQ(atf_config_get(v->lc), test_value);
        else
            ATF_CHECK(strcmp(atf_config_get(v->lc), test_value) != 0);
    }
}

/* ---------------------------------------------------------------------
 * Test cases for the free functions.
 * --------------------------------------------------------------------- */

ATF_TC(get);
ATF_TC_HEAD(get, tc)
{
    atf_tc_set_md_var(tc, "descr", "Tests the atf_config_get function");
}
ATF_TC_BODY(get, tc)
{
    const struct varnames *v;

    /* Unset all known environment variables and make sure the built-in
     * values do not match the bogus value we will use for testing. */
    unset_all();
    __atf_config_reinit();
    for (v = all_vars; v->lc != NULL; v++)
        ATF_CHECK(strcmp(atf_config_get(v->lc), test_value) != 0);

    /* Make sure empty values in the environment are not considered. */
    for (v = all_vars; v->lc != NULL; v++)
        RE(atf_env_set(v->uc, ""));
    __atf_config_reinit();
    for (v = all_vars; v->lc != NULL; v++)
        ATF_CHECK(strlen(atf_config_get(v->lc)) > 0);

    /* Check if every variable is recognized individually. */
    for (v = all_vars; v->lc != NULL; v++) {
        unset_all();
        RE(atf_env_set(v->uc, test_value));
        __atf_config_reinit();
        compare_one(v->lc, test_value);
    }
}

/* ---------------------------------------------------------------------
 * Main.
 * --------------------------------------------------------------------- */

ATF_TP_ADD_TCS(tp)
{
    ATF_TP_ADD_TC(tp, get);

    return atf_no_error();
}
