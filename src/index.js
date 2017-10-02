/*
 * index.js
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * Entry point for the libcss-js project.
 */

'use strict';

/*
 * These are the signatures of the C functions that need to be implemented:
 *
 * const char* js_node_name(const char* node);
 * const char* js_node_classes(const char* node);
 * const char* js_node_id(const char* node);
 * const char* js_named_ancestor_node(const char* node, const char* ancestor);
 * const char* js_named_parent_node(const char* node, const char* parent);
 * const char* js_named_sibling_node(const char* node, const char* sibling);
 * const char* js_named_generic_sibling_node(
 *   const char* node, const char* sibling);
 * const char* js_parent_node(const char* node);
 * const char* js_sibling_node(const char* node);
 * bool js_node_has_name(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_class(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_id(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_attribute(
 *   const char* node, const char* search, const char* empty_match);
 * bool js_node_has_attribute_equal(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_dashmatch(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_includes(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_prefix(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_suffix(
 *   const char* node, const char* search, const char* match);
 * bool js_node_has_attribute_substring(
 *   const char* node, const char* search, const char* match);
 * bool js_node_is_root(
 *   const char* node, const char* empty_search, const char* empty_match);
 * int32_t js_node_count_siblings(const char* node, bool same_name, bool after);
 * bool js_node_is_empty(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_link(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_visited(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_hover(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_active(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_focus(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_enabled(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_disabled(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_checked(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_target(
 *   const char* node, const char* empty_search, const char* empty_match);
 * bool js_node_is_lang(
 *   const char* node, const char* search, const char* empty_match);
 *
 * const int js_ua_font_size();
 */
