/*
 * libcss-js.h
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * Handler functions for libcss.
 * Based on the source code of netsurf and libcss.
 * Base files available at http://source.netsurf-browser.org
 *
 * Part of the libcss-js project.
 */

#ifndef _LIBCSS_JS_H_
#define _LIBCSS_JS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libcss/libcss.h>
#include "libcss/test/dump_computed.h"

#define UNUSED(x) ((x) = (x))

typedef enum css_js_error {
	CSS_JS_OK		=  0,
	CSS_JS_ELEMENT	   =  1,
	CSS_JS_PSEUDO	    =  2,
	CSS_JS_CREATE_CTX	=  3,
	CSS_JS_CREATE_SHEET      =  4,
	CSS_JS_CREATE_STYLE      =  5,
	CSS_JS_COMPOSE_STYLE     =  6,
	CSS_JS_DESTROY_CTX       =  7,
	CSS_JS_DESTROY_SHEET     =  8,
	CSS_JS_DESTROY_STYLE     =  9,
	CSS_JS_DESTROY_NODE_DATA = 10,
	CSS_JS_LEVEL	     = 11,
	CSS_JS_ORIGIN	    = 12,
	CSS_JS_MEDIA	     = 13,
	CSS_JS_APPEND_DATA       = 14,
	CSS_JS_DATA_DONE	 = 15,
	CSS_JS_APPEND_SHEET      = 16,
	CSS_JS_HANDLER_LENGTH    = 17
} css_js_error;

/*
 * Linked list of pointers to libcss_node_data.
 * The ID is the handler for the node.
 */
struct css_js_node {
	lwc_string* id;
	void* data;
	css_select_results* sr;
	struct css_js_node* next;
};
typedef struct css_js_node css_js_node;

/*
 * Linked list of pointers to css_stylesheet.
 * Its purpose is to track all stylesheets so they can be freed.
 */
struct stylesheet_list {
	css_stylesheet* sheet;
	struct stylesheet_list* next;
};
typedef struct stylesheet_list stylesheet_list;

/*
 * Resets the selection context (i.e. removes all added CSS stylesheets).
 */
css_js_error reset_ctx (void);

/*
 * Adds a CSS stylesheet to the selection context.
 * Parameters:
 * 	css_string: a string of CSS text to be added to the selection context.
 * 	level: a string that specifies the language level of the CSS code.
 * 		Accepted values are "1", "2", "2.1" and "3".
 * 	origin: a string that specifies the origin of the stylesheet.
 * 		Accepted values are "ua", "UA", "user agent", "user-agent",
 * 		"user" and "author".
 * 	media: a string that specifies to which media the stylesheet
 * 		should apply.
 * 		Values should be comma-separated. No whitespaces are allowed.
 * 		Accepted values are "all", "tv", "tty", "aural", "print",
 * 		"screen", "speech", "braille", "embossed", "handheld" and
 * 		"projection".
 * 	url: a string that specifies the base URL for the stylesheet.
 * 		If none, should be "" (empty string).
 */
css_js_error add_stylesheet (const char* css_string, const char* level,
		const char* origin, const char* media, const char* url);

/*
 * Gets the computed style for an element as a string identical to libcss's
 * selection test output.
 * Parameters:
 * 	element: a string that uniquely identifies the element being queried.
 * 	pseudo: a string that specifies the pseudo-element being queried.
 * 		Accepted values are "none", "first-line", "first-letter",
 * 		"before" and "after".
 * 	origin: a string that specifies the origin of the stylesheet.
 * 		Accepted values are "ua", "UA", "user agent", "user-agent",
 * 		"user" and "author".
 * 	media: a string that specifies the media being queried.
 * 		Accepted values are "all", "tv", "tty", "aural", "print",
 * 		"screen", "speech", "braille", "embossed", "handheld" and
 * 		"projection".
 * 	inline_style: a string of CSS to be parsed as inline style.
 * 		If none, should be "" (empty string).
 * 	results: a pointer to a buffer in the heap of size <len>.
 * 	len: the size of the aforementioned buffer. Must be large enough
 * 		to fit the results. In libcss's original code, the value
 * 		of 8192 is used.
 */
css_js_error get_style (const char* element, const char* pseudo,
			const char* media, const char* inline_style,
			char* results, size_t len);

/*
 * Sets the Javascript handler functions.
 * Parameters:
 * 	arr: an array of function pointers cast to uint64_t*.
 * 	len: the count of function pointers. Currently, must be 33.
 *
 * The array of functions must contain the following pointers, in order:
 * char* (*js_node_name)(const char* node);
 * char* (*js_node_classes)(const char* node);
 * char* (*js_node_id)(const char* node);
 * char* (*js_named_ancestor_node)(const char* node, const char* ancestor);
 * char* (*js_named_parent_node)(const char* node, const char* parent);
 * char* (*js_named_sibling_node)(const char* node, const char* sibling);
 * char* (*js_named_generic_sibling_node)(
 * 		const char* node, const char* sibling);
 * char* (*js_parent_node)(const char* node);
 * char* (*js_sibling_node)(const char* node);
 * bool (*js_node_has_name)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_class)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_id)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_attribute)(
 * 		const char* node, const char* search, const char* empty_match);
 * bool (*js_node_has_attribute_equal)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_dashmatch)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_includes)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_prefix)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_suffix)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_has_attribute_substring)(
 * 		const char* node, const char* search, const char* match);
 * bool (*js_node_is_root)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * int32_t (*js_node_count_siblings)(const char* node, bool same_name,
 * 		bool after);
 * bool (*js_node_is_empty)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_link)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_visited)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_hover)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_active)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_focus)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_enabled)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_disabled)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_checked)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_target)(
 * 		const char* node, const char* empty_search,
 * 		const char* empty_match);
 * bool (*js_node_is_lang)(
 * 		const char* node, const char* search, const char* empty_match);
 * int32_t (*js_ua_font_size)(void);
 */ 
css_js_error set_handlers(uint64_t* arr, size_t len);

#endif
