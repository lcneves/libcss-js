/*
 * libcss-handler.c
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * Handler functions for libcss.
 * Based on the source code of netsurf and libcss.
 * Base files available at http://source.netsurf-browser.org
 *
 * Part of the libcss-js project.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libcss/libcss.h>
#include "libcss/test/dump_computed.h"

css_error css_resolve_url(void *pw, const char *base,
		lwc_string *rel, lwc_string **abs);
void add_stylesheet (const char* css_string, const char* level,
		const char* charset, const char* url);

static css_error node_name(void *pw, void *node, css_qname *qname);
static css_error node_classes(void *pw, void *node,
		lwc_string ***classes, uint32_t *n_classes);
static css_error node_id(void *pw, void *node, lwc_string **id);
static css_error named_parent_node(void *pw, void *node,
		const css_qname *qname, void **parent);
static css_error named_sibling_node(void *pw, void *node,
		const css_qname *qname, void **sibling);
static css_error named_generic_sibling_node(void *pw, void *node,
		const css_qname *qname, void **sibling);
static css_error parent_node(void *pw, void *node, void **parent);
static css_error sibling_node(void *pw, void *node, void **sibling);
static css_error node_has_name(void *pw, void *node,
		const css_qname *qname, bool *match);
static css_error node_has_class(void *pw, void *node,
		lwc_string *name, bool *match);
static css_error node_has_id(void *pw, void *node,
		lwc_string *name, bool *match);
static css_error node_has_attribute(void *pw, void *node,
		const css_qname *qname, bool *match);
static css_error node_has_attribute_equal(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match);
static css_error node_has_attribute_dashmatch(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match);
static css_error node_has_attribute_includes(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match);
static css_error node_has_attribute_prefix(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match);
static css_error node_has_attribute_suffix(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match);
static css_error node_has_attribute_substring(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match);
static css_error node_is_root(void *pw, void *node, bool *match);
static css_error node_count_siblings(void *pw, void *node,
		bool same_name, bool after, int32_t *count);
static css_error node_is_empty(void *pw, void *node, bool *match);
static css_error node_is_link(void *pw, void *node, bool *match);
static css_error node_is_hover(void *pw, void *node, bool *match);
static css_error node_is_active(void *pw, void *node, bool *match);
static css_error node_is_focus(void *pw, void *node, bool *match);
static css_error node_is_enabled(void *pw, void *node, bool *match);
static css_error node_is_disabled(void *pw, void *node, bool *match);
static css_error node_is_checked(void *pw, void *node, bool *match);
static css_error node_is_target(void *pw, void *node, bool *match);
static css_error node_is_lang(void *pw, void *node,
		lwc_string *lang, bool *match);
static css_error node_presentational_hint(void *pw, void *node,
		uint32_t *nhints, css_hint **hints);
static css_error ua_default_for_property(void *pw, uint32_t property,
		css_hint *hint);
static css_error set_libcss_node_data(void *pw, void *node,
		void *libcss_node_data);
static css_error get_libcss_node_data(void *pw, void *node,
		void **libcss_node_data);

typedef enum css_js_error {
	CSS_JS_OK             =  0,
	CSS_JS_ELEMENT        =  1,
	CSS_JS_PSEUDO         =  2,
	CSS_JS_CREATE_CTX     =  3,
	CSS_JS_CREATE_SHEET   =  4,
	CSS_JS_CREATE_STYLE   =  5,
	CSS_JS_DESTROY_CTX    =  6,
	CSS_JS_DESTROY_SHEET  =  7,
	CSS_JS_DESTROY_STYLE  =  8,
	CSS_JS_APPEND_DATA    =  9,
	CSS_JS_DATA_DONE      = 10,
	CSS_JS_APPEND_SHEET   = 11,
	CSS_JS_HANDLER_LENGTH = 12
} css_js_error;

/*
 * Pointers for handler functions to be received from the Javascript end.
 */
const char* (*js_node_name)(const char* node) = NULL;
const char* (*js_node_classes)(const char* node) = NULL;
const char* (*js_node_id)(const char* node) = NULL;
const char* (*js_named_ancestor_node)(const char* node, const char* ancestor)
	= NULL;
const char* (*js_named_parent_node)(const char* node, const char* parent)
	= NULL;
const char* (*js_named_sibling_node)(const char* node, const char* sibling)
	= NULL;
const char* (*js_named_generic_sibling_node)(
		const char* node, const char* sibling) = NULL;
const char* (*js_parent_node)(const char* node) = NULL;
const char* (*js_sibling_node)(const char* node) = NULL;
bool (*js_node_has_name)(
		const char* node, const char* search, const char* empty_match
		) = NULL;
bool (*js_node_has_class)(
		const char* node, const char* search, const char* empty_match
		) = NULL;
bool (*js_node_has_id)(
		const char* node, const char* search, const char* empty_match
		) = NULL;
bool (*js_node_has_attribute)(
		const char* node, const char* search, const char* empty_match
		) = NULL;
bool (*js_node_has_attribute_equal)(
		const char* node, const char* search, const char* match) = NULL;
bool (*js_node_has_attribute_dashmatch)(
		const char* node, const char* search, const char* match) = NULL;
bool (*js_node_has_attribute_includes)(
		const char* node, const char* search, const char* match) = NULL;
bool (*js_node_has_attribute_prefix)(
		const char* node, const char* search, const char* match) = NULL;
bool (*js_node_has_attribute_suffix)(
		const char* node, const char* search, const char* match) = NULL;
bool (*js_node_has_attribute_substring)(
		const char* node, const char* search, const char* match) = NULL;
bool (*js_node_is_root)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
int32_t (*js_node_count_siblings)(const char* node, bool same_name, bool after
		) = NULL;
bool (*js_node_is_empty)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_link)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_visited)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_hover)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_active)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_focus)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_enabled)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_disabled)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_checked)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_target)(
		const char* node, const char* empty_search,
		const char* empty_match) = NULL;
bool (*js_node_is_lang)(
		const char* node, const char* search, const char* empty_match
		) = NULL;
const int32_t (*js_ua_font_size)(void) = NULL;
#define HANDLER_LEN 33;

css_js_error set_handlers(uint32_t* ptr, size_t len) {
	if (len != HANDLER_LEN)
		return CSS_JS_HANDLER_LENGTH;

	js_node_name = (const char* (*)(const char*)) ptr[0];
	js_node_classes = (const char* (*)(const char*)) ptr[1];
	js_node_id = (const char* (*)(const char*)) ptr[2];
	js_named_ancestor_node = (const char* (*)(
				const char*, const char*)) ptr[3];
	js_named_parent_node = (const char* (*)(
				const char*, const char*)) ptr[4];
	js_named_sibling_node = (const char* (*)(
				const char*, const char*)) ptr[5];
	js_named_generic_sibling_node = (const char* (*)(
				const char*, const char*)) ptr[6];
	js_parent_node = (const char* (*)(const char*)) ptr[7];
	js_sibling_node = (const char* (*)(const char*)) ptr[8];
	js_node_has_name = (bool (*)(const char*, const char*, const char*))
		ptr[9];
	js_node_has_class = (bool (*)(const char*, const char*, const char*))
		ptr[10];
	js_node_has_id = (bool (*)(const char*, const char*, const char*))
		ptr[11];
	js_node_has_attribute = (bool (*)(const char*, const char*,
				const char*)) ptr[12];
	js_node_has_attribute_equal = (bool (*)(const char*, const char*,
				const char*)) ptr[13];
	js_node_has_attribute_dashmatch = (bool (*)(const char*, const char*,
				const char*)) ptr[14];
	js_node_has_attribute_includes = (bool (*)(const char*, const char*,
				const char*)) ptr[15];
	js_node_has_attribute_prefix = (bool (*)(const char*, const char*,
				const char*)) ptr[16];
	js_node_has_attribute_suffix = (bool (*)(const char*, const char*,
				const char*)) ptr[17];
	js_node_has_attribute_substring = (bool (*)(const char*, const char*,
				const char*)) ptr[18];
	js_node_is_root = (bool (*)(const char*, const char*, const char*))
		ptr[19];
	js_node_count_siblings = (int32_t (*)(const char*, bool, bool))
		ptr[20];
	js_node_is_empty = (bool (*)(const char*, const char*, const char*))
		ptr[21];
	js_node_is_link = (bool (*)(const char*, const char*, const char*))
		ptr[22];
	js_node_is_visited = (bool (*)(const char*, const char*, const char*))
		ptr[23];
	js_node_is_hover = (bool (*)(const char*, const char*, const char*))
		ptr[24];
	js_node_is_active = (bool (*)(const char*, const char*, const char*))
		ptr[25];
	js_node_is_focus = (bool (*)(const char*, const char*, const char*))
		ptr[26];
	js_node_is_enabled = (bool (*)(const char*, const char*, const char*))
		ptr[27];
	js_node_is_disabled = (bool (*)(const char*, const char*, const char*))
		ptr[28];
	js_node_is_checked = (bool (*)(const char*, const char*, const char*))
		ptr[29];
	js_node_is_target = (bool (*)(const char*, const char*, const char*))
		ptr[30];
	js_node_is_lang = (bool (*)(const char*, const char*, const char*))
		ptr[31];
	js_ua_font_size = (const int32_t (*)(void)) ptr[32];

	return CSS_JS_OK;
}

css_select_ctx* select_ctx = NULL;

/**
 * Selection callback table for libcss
 */
static css_select_handler selection_handler = {
	CSS_SELECT_HANDLER_VERSION_1,

	node_name,
	node_classes,
	node_id,
	named_ancestor_node,
	named_parent_node,
	named_sibling_node,
	named_generic_sibling_node,
	parent_node,
	sibling_node,
	node_has_name,
	node_has_class,
	node_has_id,
	node_has_attribute,
	node_has_attribute_equal,
	node_has_attribute_dashmatch,
	node_has_attribute_includes,
	node_has_attribute_prefix,
	node_has_attribute_suffix,
	node_has_attribute_substring,
	node_is_root,
	node_count_siblings,
	node_is_empty,
	node_is_link,
	node_is_visited,
	node_is_hover,
	node_is_active,
	node_is_focus,
	node_is_enabled,
	node_is_disabled,
	node_is_checked,
	node_is_target,
	node_is_lang,
	node_presentational_hint,
	ua_default_for_property,
	compute_font_size,
	set_libcss_node_data,
	get_libcss_node_data,
};

css_error resolve_url(
		void *pw, const char *base, lwc_string *rel, lwc_string **abs
		)
{
	const char* base_str = base;
	const size_t base_s = strlen(base);
	const char* rel_str = lwc_string_data(rel_str);
	const size_t rel_s = strlen(rel_str);

	if (base_str[base_s] == "/")
		base_str[base_s] = "\0";

	char abs_str[base_s + rel_s + 2];
	strcpy(abs_str, base_str);
	strcat(abs_str, "/");
	strcat(abs_str, rel_str);

	lwc_intern_string(abs_str, strlen(abs_str), abs);
	return CSS_OK;
}

css_js_error reset_ctx()
{
	css_error code;

	if (select_ctx != NULL) {
		for (uint32_t i = 0; i < ctx->n_sheets; ++i) {
			code = css_stylesheet_destroy(ctx->sheets[index].sheet);
			if (code != CSS_OK)
				return CSS_JS_DESTROY_SHEET;
		}

		code = css_select_ctx_destroy(select_ctx);
		if (code != CSS_OK)
			return CSS_JS_DESTROY_CTX;
	}

	code = css_select_ctx_create(&select_ctx);
	if (code != CSS_OK)
		return CSS_JS_CREATE_CTX;

	return CSS_JS_OK;
}

css_js_error add_stylesheet (
		const char* css_string,
		const char* level,
		const char* charset,
		const char* url
		)
{
	css_error code;

	css_language_level css_level;
	if (strcmp(level, "1") == 0)
		css_level = CSS_LEVEL_1;
	else if (strcmp(level, "2") == 0)
		css_level = CSS_LEVEL_2;
	else if (strcmp(level, "2.1") == 0)
		css_level = CSS_LEVEL_21;
	else
		css_level = CSS_LEVEL_3; /* Default */

	css_stylesheet_params params;
	params.params_version = CSS_STYLESHEET_PARAMS_VERSION_1;
	params.level = css_level;
	params.charset = charset;
	params.url = url;
	params.title = NULL;
	params.allow_quirks = false;
	params.inline_style = false;
	params.resolve = resolve_url;
	params.resolve_pw = NULL;
	params.import = NULL;
	params.import_pw = NULL;
	params.color = NULL;
	params.color_pw = NULL;
	params.font = NULL;
	params.font_pw = NULL;

	css_stylesheet *sheet;

	code = css_stylesheet_create(&params, &sheet);
	if (code != CSS_OK)
		return CSS_JS_CREATE_SHEET;

	code = css_stylesheet_append_data(
			sheet,
			(const uint8_t *) data,
			sizeof data
			);
	if (code != CSS_OK && code != CSS_NEEDDATA)
		return CSS_JS_APPEND_DATA;

	code = css_stylesheet_data_done(sheet);
	if (code != CSS_OK)
		return CSS_JS_DATA_DONE;

	if (select_ctx == NULL) {
		css_js_error js_error = reset_ctx();
		if (js_error != CSS_JS_OK)
			return js_error;
	}

	code = css_select_ctx_append_sheet(
			select_ctx,
			sheet,
			CSS_ORIGIN_AUTHOR,
			CSS_MEDIA_ALL
	);
	if (code != CSS_OK)
		return CSS_JS_APPEND_SHEET;

	return CSS_JS_OK;
}

/*
 * Warning: if inline_style is set, the resulting stylesheet will not
 * be destroyed! This implementation leaks memory.
 */
css_js_error build_style(lwc_string* node,
		      const char* inline_style,
		      css_select_results** results)
{
	css_error code;

	const css_stylesheet* in_style;
	if (inline_style == NULL || *inline_style == "\0")
		in_style = NULL;
	else
	{
		css_stylesheet_params params;
		params.params_version = CSS_STYLESHEET_PARAMS_VERSION_1;
		params.level = css_level;
		params.charset = "UTF-8";
		params.url = NULL;
		params.title = NULL;
		params.allow_quirks = false;
		params.inline_style = true;
		params.resolve = NULL;
		params.resolve_pw = NULL;
		params.import = NULL;
		params.import_pw = NULL;
		params.color = NULL;
		params.color_pw = NULL;
		params.font = NULL;
		params.font_pw = NULL;

		css_stylesheet* sheet;
		code = css_stylesheet_create(&params, &sheet);
		if (code != CSS_OK)
			return CSS_JS_CREATE_SHEET;

		code = css_stylesheet_append_data(
				sheet,
				(const uint8_t *) inline_style,
				sizeof inline_style
				);
		if (code != CSS_OK && CODE != CSS_NEEDDATA)
			return CSS_JS_APPEND_DATA;

		code = css_stylesheet_data_done(sheet);
		if (code != CSS_OK)
			return CSS_JS_DATA_DONE;

		in_style = sheet;
	}

	code = css_select_style(
			select_ctx,
			node,
			CSS_MEDIA_SCREEN,
			in_style,
			selection_handler,
			NULL,
			results
			);
	if (code != CSS_OK)
		return CSS_JS_CREATE_STYLE;

	return CSS_JS_OK;
}

css_js_error get_style (const char* element,
			const char* pseudo,
			const char* inline_style,
			char* results,
			size_t len)
{
	css_error code;
	css_js_error js_code;

	css_pseudo_element pseudo_code;
	if (strcmp(pseudo, "none") == 0)
		pseudo_code = CSS_PSEUDO_ELEMENT_NONE;
	else if (strcmp(pseudo, "first-line") == 0)
		pseudo_code = CSS_PSEUDO_FIRST_LINE;
	else if (strcmp(pseudo, "first-letter") == 0)
		pseudo_code = CSS_PSEUDO_FIRST_LETTER;
	else if (strcmp(pseudo, "before") == 0)
		pseudo_code = CSS_PSEUDO_BEFORE;
	else if (strcmp(pseudo, "after") == 0)
		pseudo_code = CSS_PSEUDO_AFTER;
	else
		return CSS_JS_PSEUDO;

	lwc_string* node;
	lwc_intern_string(element, strlen(element), &node);
	css_select_results* style;
	js_code = build_style(node, inline_style, &style);
	if (js_code != CSS_JS_OK)
		return js_code;

	lwc_string_unref(node);

	dump_computed_style(style->styles[pseudo_code], results, &len);

	code = css_select_results_destroy(style);
	if (code != CSS_OK)
		return CSS_JS_DESTROY_STYLE;

	return CSS_JS_OK;
}

const int UA_FONT_SIZE = (*js_ua_font_size)();

/**
 * Font size computation callback for libcss
 *
 * \param pw      Computation context
 * \param parent  Parent font size (absolute)
 * \param size    Font size to compute
 * \return CSS_OK on success
 *
 * \post \a size will be an absolute font size
 *
 * Slightly adapted from NetSurf's original implementation.
 */
css_error compute_font_size(void *pw, const css_hint *parent,
		css_hint *size)
{
	/**
	 * Table of font-size keyword scale factors
	 *
	 * These are multiplied by the configured default font size
	 * to produce an absolute size for the relevant keyword
	 */
	static const css_fixed factors[] = {
		FLTTOFIX(0.5625), /* xx-small */
		FLTTOFIX(0.6250), /* x-small */
		FLTTOFIX(0.8125), /* small */
		FLTTOFIX(1.0000), /* medium */
		FLTTOFIX(1.1250), /* large */
		FLTTOFIX(1.5000), /* x-large */
		FLTTOFIX(2.0000)  /* xx-large */
	};
	css_hint_length parent_size;

	/* Grab parent size, defaulting to medium if none */
	if (parent == NULL) {
		parent_size.value = FDIV(FMUL(factors[CSS_FONT_SIZE_MEDIUM - 1],
					INTTOFIX(UA_FONT_SIZE)),
				INTTOFIX(10));
		parent_size.unit = CSS_UNIT_PT;
	} else {
		assert(parent->status == CSS_FONT_SIZE_DIMENSION);
		assert(parent->data.length.unit != CSS_UNIT_EM);
		assert(parent->data.length.unit != CSS_UNIT_EX);
		assert(parent->data.length.unit != CSS_UNIT_PCT);

		parent_size = parent->data.length;
	}

	assert(size->status != CSS_FONT_SIZE_INHERIT);

	if (size->status < CSS_FONT_SIZE_LARGER) {
		/* Keyword -- simple */
		size->data.length.value = FDIV(FMUL(factors[size->status - 1],
					INTTOFIX(UA_FONT_SIZE)), F_10);
		size->data.length.unit = CSS_UNIT_PT;
	} else if (size->status == CSS_FONT_SIZE_LARGER) {
		/** \todo Step within table, if appropriate */
		size->data.length.value =
			FMUL(parent_size.value, FLTTOFIX(1.2));
		size->data.length.unit = parent_size.unit;
	} else if (size->status == CSS_FONT_SIZE_SMALLER) {
		/** \todo Step within table, if appropriate */
		size->data.length.value =
			FDIV(parent_size.value, FLTTOFIX(1.2));
		size->data.length.unit = parent_size.unit;
	} else if (size->data.length.unit == CSS_UNIT_EM ||
			size->data.length.unit == CSS_UNIT_EX) {
		size->data.length.value =
			FMUL(size->data.length.value, parent_size.value);

		if (size->data.length.unit == CSS_UNIT_EX) {
			/* 1ex = 0.6em in NetSurf */
			size->data.length.value = FMUL(size->data.length.value,
					FLTTOFIX(0.6));
		}

		size->data.length.unit = parent_size.unit;
	} else if (size->data.length.unit == CSS_UNIT_PCT) {
		size->data.length.value = FDIV(FMUL(size->data.length.value,
					parent_size.value), INTTOFIX(100));
		size->data.length.unit = parent_size.unit;
	}

	size->status = CSS_FONT_SIZE_DIMENSION;
	return CSS_OK;
}

lwc_string* NULL_STR;
lwc_intern_string("", 0, &NULL_STR);

/*
 * Generic get function to be used by callbacks. Must yield string.
 */
css_error get_string (
		void *node,
		const char* (*js_fun)(const char*),
		lwc_string** ret
		)
{
	lwc_string* n = (lwc_string*) node;
	const char* node_string = lwc_string_data(n);

	const char* js_results =
		(*js_fun)(node_string);

	if (*js_results == "\0")
	{
		*ret = NULL;
	}
	else
	{
		lwc_string *results;
		lwc_intern_string(js_results, strlen(js_results), &results);
		*ret = lwc_string_ref(results);
	}

	return CSS_OK;
}

/*
 * Generic search function to be used by callbacks. Must yield boolean.
 */
css_error match_bool (
		void *node,
		lwc_string* search_parameter,
		lwc_string* match_parameter,
		bool (*js_fun)(const char*, const char*, const char*),
		bool* ret
		)
{
	lwc_string* n = (lwc_string*) node;
	const char* node_string = lwc_string_data(n);

	lwc_string* s = search_parameter;
	const char* search_string = s == NULL
		? lwc_string_data(NULL_STR)
		: lwc_string_data(s);

	lwc_string* m = match_parameter;
	const char* match_string = m == NULL
		? lwc_string_data(NULL_STR)
		: lwc_string_data(m);

	*ret = (*js_fun)(node_string, search_string, match_string);
	return CSS_OK;
}

/*
 * Generic search function to be used by callbacks. Must yield string.
 */
css_error match_string (
		void *node,
		lwc_string* search_parameter,
		const char* (*js_fun)(const char*, const char*),
		lwc_string** ret
		)
{
	lwc_string* n = node;
	const char* node_string = lwc_string_data(n);
	lwc_string* s = search_parameter;
	const char* match_string = lwc_string_data(s);

	const char* js_results =
		(*js_fun)(node_string, match_string);

	if (*js_results == "\0")
	{
		*ret = NULL;
	}
	else
	{
		lwc_string *results;
		lwc_intern_string(js_results, strlen(js_results), &results);
		*ret = lwc_string_ref(results);
	}

	return CSS_OK;
}

/*
 * Not sure this is needed for node_name
 void set_namespace (css_qname* qname, const char* ns)
 {
 lwc_string *namespace_ptr;
 lwc_intern_string(ns, strlen(ns), &namespace_ptr);
 qname->ns = namespace_ptr;
 }
 */


/******************************************************************************
 * Style selection callbacks                                                  *
 ******************************************************************************/

/**
 * Callback to retrieve a node's name.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Pointer to location to receive node name
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 */
css_error node_name(void *pw, void *node, css_qname *qname)
{
	/*  set_namespace(qname, "names"); *Is this needed? */

	return get_string(node, js_node_name, &(qname->name));
}

/**
 * Callback to retrieve a node's classes.
 *
 * \param pw         HTML document
 * \param node       DOM node
 * \param classes    Pointer to location to receive class name array
 * \param n_classes  Pointer to location to receive length of class name array
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \note The returned array will be destroyed by libcss. Therefore, it must
 *       be allocated using the same allocator as used by libcss during style
 *       selection.
 */
css_error node_classes(void *pw, void *node,
		lwc_string ***classes, uint32_t *n_classes)
{
	lwc_string* n = (lwc_string*) node;
	const char* node_string = lwc_string_data(n);

	/* This js function returns a stringified array of class strings */
	const char* js_results = (*js_node_classes)(node_string);

	/* Bail out */
	if (*js_results == NULL)
	{
		*classes = NULL;
		*n_classes = 0;
		return CSS_OK;
	}

	char* current_c = js_results;
	uint32_t n = 1;
	while(*current_c != NULL)
	{
		if (*(++current_c) == ",")
			++n;
	}

	lwc_string** ptr_array = malloc(sizeof(lwc_string*) * n);

	size_t current_class_s = strlen(js_results) + 1;
	const char current_class[current_class_s];
	int offset = 0;
	int classes_processed = 0;
	current_c = js_results;
	while (*(current_c + offset) != NULL)
	{
		switch (*(current_c + offset))
		{
			case "\"":
			case "[":
				++current_c;
				break;

			case ",":
			case "]":
				current_c += ++offset;
				current_class[offset] = "\0";
				offset = 0;
				lwc_string* class_name;
				lwc_intern_string(
						current_class,
						strlen(current_class),
						&class_name);
				ptr_array[classes_processed++ *
					sizeof(lwc_string*)] =
					lwc_string_ref(class_name);
				break;

			default:
				current_class[offset] = *(current_c + offset++);
				break;
		}
	}

	*classes = ptr_array;
	*n_classes = n;
	return CSS_OK;
}

/**
 * Callback to retrieve a node's ID.
 *
 * \param pw    HTML document
 * \param node  DOM node
 * \param id    Pointer to location to receive id value
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 */
css_error node_id(void *pw, void *node, lwc_string **id)
{
	return get_string(node, js_node_id, id);
}

/**
 * Callback to find a named ancestor node.
 *
 * \param pw        HTML document
 * \param node      DOM node
 * \param qname     Node name to search for
 * \param ancestor  Pointer to location to receive ancestor
 * \return CSS_OK.
 *
 * \post \a ancestor will contain the result, or NULL if there is no match
 */
css_error named_ancestor_node(void *pw, void *node,
		const css_qname *qname, void **ancestor)
{
	return match_string(
			node, qname->name, js_named_ancestor_node, ancestor);
}

/**
 * Callback to find a named parent node
 *
 * \param pw      HTML document
 * \param node    DOM node
 * \param qname   Node name to search for
 * \param parent  Pointer to location to receive parent
 * \return CSS_OK.
 *
 * \post \a parent will contain the result, or NULL if there is no match
 */
css_error named_parent_node(void *pw, void *node,
		const css_qname *qname, void **parent)
{
	return match_string(node, qname->name, js_named_parent_node, ancestor);
}

/**
 * Callback to find a named sibling node.
 *
 * NOTE: According to the NetSurf source code, this function returns a node
 * only if the Node.previousSibling is the searched node!
 *
 * \param pw       HTML document
 * \param node     DOM node
 * \param qname    Node name to search for
 * \param sibling  Pointer to location to receive sibling
 * \return CSS_OK.
 *
 * \post \a sibling will contain the result, or NULL if there is no match
 */
css_error named_sibling_node(void *pw, void *node,
		const css_qname *qname, void **sibling)
{
	return match_string(node, qname->name, js_named_sibling_node, sibling);
}

/**
 * Callback to find a named generic sibling node.
 *
 * NOTE: According to the NetSurf source code, this function returns any
 * sibling node that matches the search!
 *
 * \param pw       HTML document
 * \param node     DOM node
 * \param qname    Node name to search for
 * \param sibling  Pointer to location to receive ancestor
 * \return CSS_OK.
 *
 * \post \a sibling will contain the result, or NULL if there is no match
 */
css_error named_generic_sibling_node(void *pw, void *node,
		const css_qname *qname, void **sibling)
{
	return
		match_string(node, qname->name,
				js_named_generic_sibling_node, sibling);
}

/**
 * Callback to retrieve the parent of a node.
 *
 * \param pw      HTML document
 * \param node    DOM node
 * \param parent  Pointer to location to receive parent
 * \return CSS_OK.
 *
 * \post \a parent will contain the result, or NULL if there is no match
 */
css_error parent_node(void *pw, void *node, void **parent)
{
	return get_string(node, js_parent_node, parent);
}

/**
 * Callback to retrieve the preceding sibling of a node.
 *
 * \param pw       HTML document
 * \param node     DOM node
 * \param sibling  Pointer to location to receive sibling
 * \return CSS_OK.
 *
 * \post \a sibling will contain the result, or NULL if there is no match
 */
css_error sibling_node(void *pw, void *node, void **sibling)
{
	return get_string(node, js_sibling_node, sibling);
}

/**
 * Callback to determine if a node has the given name.
 *
 * NOTE: Element names are case insensitive in HTML
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_name(void *pw, void *node,
		const css_qname *qname, bool *match)
{
	return match_bool(node, qname->name, NULL, js_node_has_name, match);
}

/**
 * Callback to determine if a node has the given class.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param name   Name to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_class(void *pw, void *node,
		lwc_string *name, bool *match)
{
	return match_bool(node, name, NULL, js_node_has_class, match);
}

/**
 * Callback to determine if a node has the given id.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param name   Name to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_id(void *pw, void *node,
		lwc_string *name, bool *match)
{
	return match_bool(node, name, NULL, js_node_has_id, match);
}

/**
 * Callback to determine if a node has an attribute with the given name.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_attribute(void *pw, void *node,
		const css_qname *qname, bool *match)
{
	return match_bool(node, qname->name, NULL,
			js_node_has_attribute, match);
}

/**
 * Callback to determine if a node has an attribute with given name and value.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param value  Value to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_attribute_equal(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match)
{
	return match_bool(node, qname->name,
			value, js_node_has_attribute_equal, match);
}

/**
 * Callback to determine if a node has an attribute with the given name whose
 * value dashmatches that given.
 *
 * NOTE: Matches exact matches (case-insensitive) and matches that the observed
 * value equals the expected value plus a dash ("-").
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param value  Value to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_attribute_dashmatch(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match)
{
	return match_bool(node, qname->name, value,
			js_node_has_attribute_dashmatch, match);
}

/**
 * Callback to determine if a node has an attribute with the given name whose
 * value includes that given.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param value  Value to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_attribute_includes(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match)
{
	return match_bool(node, qname->name, value,
			js_node_has_attribute_includes, match);
}

/**
 * Callback to determine if a node has an attribute with the given name whose
 * value has the prefix given.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param value  Value to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_attribute_prefix(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match)
{
	return match_bool(node, qname->name, value,
			js_node_has_attribute_prefix, match);
}

/**
 * Callback to determine if a node has an attribute with the given name whose
 * value has the suffix given.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param value  Value to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_attribute_suffix(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match)
{
	return match_bool(node, qname->name, value,
			js_node_has_attribute_suffix, match);
}

/**
 * Callback to determine if a node has an attribute with the given name whose
 * value contains the substring given.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param qname  Name to match
 * \param value  Value to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK on success,
 *         CSS_NOMEM on memory exhaustion.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_has_attribute_substring(void *pw, void *node,
		const css_qname *qname, lwc_string *value,
		bool *match)
{
	return match_bool(node, qname->name, value,
			js_node_has_attribute_substring, match);
}

/**
 * Callback to determine if a node is the root node of the document.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_is_root(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_root, match);
}

/**
 * Callback to count a node's siblings.
 *
 * \param pw         HTML document
 * \param n          DOM node
 * \param same_name  Only count siblings with the same name, or all
 * \param after      Count anteceding instead of preceding siblings
 * \param count      Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a count will contain the number of siblings
 */
css_error node_count_siblings(void *pw, void *n, bool same_name,
		bool after, int32_t *count)
{
	lcw_string* node = n;
	const char* node_string = lwc_string_data(n);

	*count = (int32_t) (*js_node_count_siblings)(
			node_string, same_name, after);
	return CSS_OK;
}

/**
 * Callback to determine if a node is empty.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node is empty and false otherwise.
 */
css_error node_is_empty(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_empty, match);
}

/**
 * Callback to determine if a node is a linking element.
 *
 * NOTE: In NetSurf, element must be <a> and have a href property.
 *
 * \param pw     HTML document
 * \param n      DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_is_link(void *pw, void *n, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_link, match);
}

/**
 * Callback to determine if a node is a linking element whose target has been
 * visited.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_is_visited(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_visited, match);
}

/**
 * Callback to determine if a node is currently being hovered over.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_is_hover(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_hover, match);
}

/**
 * Callback to determine if a node is currently activated.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_is_active(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_active, match);
}

/**
 * Callback to determine if a node has the input focus.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_is_focus(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_focus, match);
}

/**
 * Callback to determine if a node is enabled.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match with contain true if the node is enabled and false otherwise.
 */
css_error node_is_enabled(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_enabled, match);
}

/**
 * Callback to determine if a node is disabled.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match with contain true if the node is disabled and false otherwise.
 */
css_error node_is_disabled(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_disabled, match);
}

/**
 * Callback to determine if a node is checked.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match with contain true if the node is checked and false otherwise.
 */
css_error node_is_checked(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_checked, match);
}

/**
 * Callback to determine if a node is the target of the document URL.
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match with contain true if the node matches and false otherwise.
 */
css_error node_is_target(void *pw, void *node, bool *match)
{
	return match_bool(node, NULL, NULL, js_node_is_target, match);
}

/**
 * Callback to determine if a node has the given language
 *
 * \param pw     HTML document
 * \param node   DOM node
 * \param lang   Language specifier to match
 * \param match  Pointer to location to receive result
 * \return CSS_OK.
 *
 * \post \a match will contain true if the node matches and false otherwise.
 */
css_error node_is_lang(void *pw, void *node,
		lwc_string *lang, bool *match)
{
	return match_bool(node, lang, NULL, js_node_is_lang, match);
}

/*
 * NOTE: Somehow apparently NetSurf doesn't bother implementing this.
 */
css_error node_presentational_hint(void *pw, void *node,
		uint32_t *nhints, css_hint **hints)
{
	/*
	 * TODO: Maybe implement presentational hints,
	 * when we find out what it means?
	 */
	*nhints = 0;
	*hints = NULL;
	return CSS_OK;
}

/**
 * Callback to retrieve the User-Agent defaults for a CSS property.
 *
 * \param pw        HTML document
 * \param property  Property to retrieve defaults for
 * \param hint      Pointer to hint object to populate
 * \return CSS_OK       on success,
 *         CSS_INVALID  if the property should not have a user-agent default.
 */
css_error ua_default_for_property(void *pw, uint32_t property, css_hint *hint)
{
	if (property == CSS_PROP_COLOR) {
		hint->data.color = 0xff000000;
		hint->status = CSS_COLOR_COLOR;

	} else if (property == CSS_PROP_FONT_FAMILY) {
		/* TODO: Implement default font option */
		hint->data.strings = NULL;
		hint->status = CSS_FONT_FAMILY_SANS_SERIF;

	} else if (property == CSS_PROP_QUOTES) {
		/** \todo Not exactly useful :) */
		hint->data.strings = NULL;
		hint->status = CSS_QUOTES_NONE;
	} else if (property == CSS_PROP_VOICE_FAMILY) {
		/** \todo Fix this when we have voice-family done */
		hint->data.strings = NULL;
		hint->status = 0;
	} else {
		return CSS_INVALID;
	}

	return CSS_OK;
}

/*
 * Linked list of pointers to libcss_node_data.
 * The ID is the handler for the node.
 */
struct node_data {
	lwc_string* id;
	void* data;
	struct node_data* next;
};
typedef struct node_data node_data;

node_data first_node;
first_node.id = NULL;
first_node.data = NULL;
first_node.next = NULL;

node_data* get_last_node_data ()
{
	node_data* current_node = &first_node;

	while (current_node->next != NULL)
		current_node = current_node->next;

	return current_node;
}

node_data* get_node_data_by_id (lwc_string* id)
{
	node_data* current_node = &first_node;

	while (current_node->next != NULL)
	{
		current_node = current_node->next;

		bool match;
		lwc_string_isequal(*id, current_node->id, &bool);

		if (match)
			return current_node;
	}

	return NULL;
}

void append_node_data (lwc_string* id, void* new_data)
{
	node_data* new_node = malloc(sizeof(node_data));
	new_node->id = id;
	new_node-> data = new_data;
	new_node-> next = NULL;

	node_data* last_node = get_last_node_data();
	last_node->next = new_node;
}

void update_node_data (lwc_string* id, void* new_data)
{
	node_data* node = get_node_data_by_id(id);

	if (node == NULL)
		append_node_data(id, new_data);
	else
		node->data = new_data;
}


css_error set_libcss_node_data(void *pw, void *node, void *libcss_node_data)
{
	update_node_data((lwc_string*) node, libcss_node_data);
	return CSS_OK;
}

css_error get_libcss_node_data(void *pw, void *node, void **libcss_node_data)
{
	node_data* nd = get_node_data_by_id((lwc_string*) node);
	*libcss_node_data = *nd == NULL ? NULL : nd->data;
	return CSS_OK;
}
