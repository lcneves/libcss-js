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

const css = 'div { flex: 3 5 20px; width: 100px; } #div-0 { color: red; width: 70px; height: 50px; }';

libcss.init(handlers);
libcss.addSheet(css);

var computedStyle = libcss.getStyle('idzero');
console.dir(computedStyle);

