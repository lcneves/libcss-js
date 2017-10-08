/*
 * example1.js
 * Copyright 2017 Lucas Neves <lcneves@gmail.com>
 *
 * Example client for libcss-js.
 */

'use strict';

const libcss = require('../index.js');

var nodes = [
  {
    id: 'idzero',
    tag: 'div',
    attributes: [
      { attribute: 'id', value: 'div-0' },
      { attribute: 'class', value: 'divs elements' },
      { attribute: 'style', value: 'justify-content: center;' }
    ]
  },
  {
    id: 'idone',
    tag: 'p',
    attributes: []
  }
];

function getNodeById (identifier) {
  for (let node of nodes) {
    if (node.id === identifier) {
      return node;
    }
  }
}

function getProperty (identifier, property) {
  var node = getNodeById(identifier);
  if (!node)
    return null;

  return node[property];
}

var handlers = {
  getTagName: function (identifier) {
    return getProperty(identifier, 'tag');
  },
  getAttributes: function (identifier) {
    return getProperty(identifier, 'attributes');
  },
  getSiblings: function (identifier) {
    var node = getNodeById(identifier);
    return [ node ];
  },
  getAncestors: function (identifier) {
    return [];
  },
  isEmpty (identifier) {
    return true;
  }
};

libcss.init(handlers);

const css = '#div-0 { color: red; width: 70px; height: 50px; }';
const cssBis = 'div { align-content: center; }';
const cssTris = 'p { order: 1; }';
const cssQuat = 'div { flex: none; }';

libcss.addSheet(css);
libcss.addSheet(cssBis);
libcss.addSheet(cssTris);
libcss.addSheet(cssQuat);

var computedStyle = libcss.getStyle('idzero');
console.log('flex-basis: ' + computedStyle['flex-basis']);
console.log('flex-grow: ' + computedStyle['flex-grow']);
console.log('flex-shrink: ' + computedStyle['flex-shrink']);
console.log('width: ' + computedStyle['width']);
console.log('height: ' + computedStyle['height']);
console.log('color: ' + computedStyle['color']);
console.log('align-content: ' + computedStyle['align-content']);
console.log('justify-content: ' + computedStyle['justify-content']);

libcss.addSheet(cssTris);
var computedStyleP = libcss.getStyle('idone');
console.log('order: ' + computedStyleP.order);
