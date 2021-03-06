
#line 1 "libbuzzy/interpolated.c.rl"
/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>

#include "buzzy/error.h"
#include "buzzy/value.h"


/*-----------------------------------------------------------------------
 * Interpolated elements
 */

typedef int
(*bz_render_element_f)(void *user_data, struct bz_value *ctx,
                       struct cork_buffer *dest);

struct bz_interpolated_element {
    void  *user_data;
    cork_free_f  free_user_data;
    bz_render_element_f  render;
};

static struct bz_interpolated_element *
bz_interpolated_element_new(void *user_data, cork_free_f free_user_data,
                            bz_render_element_f render)
{
    struct bz_interpolated_element  *element =
        cork_new(struct bz_interpolated_element);
    element->user_data = user_data;
    element->free_user_data = free_user_data;
    element->render = render;
    return element;
}

static void
bz_interpolated_element_free(struct bz_interpolated_element *element)
{
    cork_free_user_data(element);
    free(element);
}

static int
bz_interpolated_element_render(struct bz_interpolated_element *element,
                               struct bz_value *ctx, struct cork_buffer *dest)
{
    return element->render(element->user_data, ctx, dest);
}


/*-----------------------------------------------------------------------
 * Strings
 */

static void
bz_string_element__free(void *user_data)
{
    const char  *content = user_data;
    cork_strfree(content);
}

static int
bz_string_element__render(void *user_data, struct bz_value *ctx,
                          struct cork_buffer *dest)
{
    const char  *content = user_data;
    cork_buffer_append_string(dest, content);
    return 0;
}

static struct bz_interpolated_element *
bz_string_element_new(const char *content, size_t length)
{
    const char  *copy = cork_strndup(content, length);
    return bz_interpolated_element_new
        ((void *) copy, bz_string_element__free, bz_string_element__render);
}


/*-----------------------------------------------------------------------
 * Variable references
 */

static void
bz_var_ref_element__free(void *user_data)
{
    const char  *var_name = user_data;
    cork_strfree(var_name);
}

static int
bz_var_ref_element__render(void *user_data, struct bz_value *ctx,
                           struct cork_buffer *dest)
{
    const char  *var_name = user_data;
    const char  *value;
    rie_check(value = bz_value_get_string(ctx, var_name, false));
    if (CORK_UNLIKELY(value == NULL)) {
        bz_bad_config("No variable named \"%s\"", var_name);
        return -1;
    } else {
        cork_buffer_append_string(dest, value);
        return 0;
    }
}

static struct bz_interpolated_element *
bz_var_ref_element_new(const char *var_name, size_t length)
{
    const char  *copy = cork_strndup(var_name, length);
    return bz_interpolated_element_new
        ((void *) copy, bz_var_ref_element__free, bz_var_ref_element__render);
}


/*-----------------------------------------------------------------------
 * Interpolated values
 */

struct bz_interpolated_value {
    cork_array(struct bz_interpolated_element *)  elements;
    struct cork_buffer  value;
};

static void
bz_interpolated_value__free(void *user_data)
{
    struct bz_interpolated_value  *value = user_data;
    cork_array_done(&value->elements);
    cork_buffer_done(&value->value);
    free(value);
}

static const char *
bz_interpolated_value__get(void *user_data, struct bz_value *ctx)
{
    size_t  i;
    struct bz_interpolated_value  *value = user_data;
    struct cork_buffer  *dest = &value->value;

    cork_buffer_clear(dest);
    for (i = 0; i < cork_array_size(&value->elements); i++) {
        struct bz_interpolated_element  *element =
            cork_array_at(&value->elements, i);
        rpi_check(bz_interpolated_element_render(element, ctx, dest));
    }
    return value->value.buf;
}

static void
bz_interpolated_value_add(struct bz_interpolated_value *value,
                          struct bz_interpolated_element *element)
{
    cork_array_append(&value->elements, element);
}

static int
bz_interpolated_value_parse(struct bz_interpolated_value *value,
                            const char *template_value)
{
    int  cs;
    const char  *p = template_value;
    const char  *pe = strchr(template_value, '\0');
    const char  *eof = pe;
    const char  *sstart = NULL;
    const char  *send = NULL;
    const char  *vstart = NULL;
    const char  *vend = NULL;
    struct bz_interpolated_element  *element;

    
#line 183 "libbuzzy/interpolated.c"
static const int interpolated_value_start = 4;

static const int interpolated_value_en_main = 4;


#line 189 "libbuzzy/interpolated.c"
	{
	cs = interpolated_value_start;
	}

#line 194 "libbuzzy/interpolated.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 4:
	if ( (*p) == 36 )
		goto st1;
	goto tr6;
tr6:
#line 201 "libbuzzy/interpolated.c.rl"
	{ sstart = p; }
	goto st5;
tr10:
#line 194 "libbuzzy/interpolated.c.rl"
	{
                   element = bz_string_element_new("$", 1);
                   bz_interpolated_value_add(value, element);
               }
#line 201 "libbuzzy/interpolated.c.rl"
	{ sstart = p; }
	goto st5;
tr12:
#line 188 "libbuzzy/interpolated.c.rl"
	{
                      element = bz_var_ref_element_new(vstart, vend - vstart);
                      bz_interpolated_value_add(value, element);
                  }
#line 201 "libbuzzy/interpolated.c.rl"
	{ sstart = p; }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 230 "libbuzzy/interpolated.c"
	if ( (*p) == 36 )
		goto tr9;
	goto st5;
tr9:
#line 202 "libbuzzy/interpolated.c.rl"
	{ send = p; }
#line 205 "libbuzzy/interpolated.c.rl"
	{
                     element = bz_string_element_new(sstart, send - sstart);
                     bz_interpolated_value_add(value, element);
                 }
	goto st1;
tr11:
#line 194 "libbuzzy/interpolated.c.rl"
	{
                   element = bz_string_element_new("$", 1);
                   bz_interpolated_value_add(value, element);
               }
	goto st1;
tr13:
#line 188 "libbuzzy/interpolated.c.rl"
	{
                      element = bz_var_ref_element_new(vstart, vend - vstart);
                      bz_interpolated_value_add(value, element);
                  }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 261 "libbuzzy/interpolated.c"
	switch( (*p) ) {
		case 36: goto st6;
		case 123: goto st2;
	}
	goto st0;
st0:
cs = 0;
	goto _out;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 36 )
		goto tr11;
	goto tr10;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 46: goto tr3;
		case 95: goto tr3;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr3;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
tr3:
#line 184 "libbuzzy/interpolated.c.rl"
	{ vstart = p; }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 302 "libbuzzy/interpolated.c"
	switch( (*p) ) {
		case 46: goto st3;
		case 95: goto st3;
		case 125: goto tr5;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st3;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st3;
	} else
		goto st3;
	goto st0;
tr5:
#line 185 "libbuzzy/interpolated.c.rl"
	{ vend = p; }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 325 "libbuzzy/interpolated.c"
	if ( (*p) == 36 )
		goto tr13;
	goto tr12;
	}
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 7: 
#line 188 "libbuzzy/interpolated.c.rl"
	{
                      element = bz_var_ref_element_new(vstart, vend - vstart);
                      bz_interpolated_value_add(value, element);
                  }
	break;
	case 6: 
#line 194 "libbuzzy/interpolated.c.rl"
	{
                   element = bz_string_element_new("$", 1);
                   bz_interpolated_value_add(value, element);
               }
	break;
	case 5: 
#line 202 "libbuzzy/interpolated.c.rl"
	{ send = p; }
#line 205 "libbuzzy/interpolated.c.rl"
	{
                     element = bz_string_element_new(sstart, send - sstart);
                     bz_interpolated_value_add(value, element);
                 }
	break;
#line 364 "libbuzzy/interpolated.c"
	}
	}

	_out: {}
	}

#line 217 "libbuzzy/interpolated.c.rl"


    /* A hack to suppress some unused variable warnings */
    (void) interpolated_value_en_main;

    if (CORK_UNLIKELY(cs < 
#line 378 "libbuzzy/interpolated.c"
4
#line 222 "libbuzzy/interpolated.c.rl"
)) {
        bz_bad_config("Bad interpolated value \"%s\"", template_value);
        return -1;
    }

    return 0;
}

struct bz_value *
bz_interpolated_value_new(const char *template_value)
{
    struct bz_interpolated_value  *value;

    value = cork_new(struct bz_interpolated_value);
    cork_pointer_array_init
        (&value->elements, (cork_free_f) bz_interpolated_element_free);
    cork_buffer_init(&value->value);
    cork_buffer_append(&value->value, "", 0);
    ei_check(bz_interpolated_value_parse(value, template_value));
    return bz_scalar_value_new
        (value, bz_interpolated_value__free, bz_interpolated_value__get);

error:
    bz_interpolated_value__free(value);
    return NULL;
}
