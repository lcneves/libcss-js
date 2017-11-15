# libcss-js
Parses CSS and lets you query the style of the elements in your document tree.
Uses the [NetSurf browser's LibCSS](www.netsurf-browser.org/projects/libcss/), transpiled to JavaScript.

## Installation
```
npm install libcss
```

## Usage
Consider the following document tree:
```html
<div id="parent">
  <div id="box" class="sections">
    <p id="a-paragraph">
      Some text.
    </p>
  </div>
  <div id="info" class="sections floaty" style="color: pink;">
  </div>
</div>
```
_Note: libcss-js will work with any document tree, not only HTML!_

```javascript
var libcss = require('libcss');

var handlers = { /* see below */ };
libcss.init(handlers);

var css1 = 'div { color: red; width: 70em; display: flex; }'
var css2 = '#box { color: green; } .floaty { float: left; }'
libcss.addSheet(css1);
libcss.addSheet(css2);

var style = libcss.getStyle('a-paragraph');
console.log(style['color']);
// 'green'
```

### libcss.init(&lt;object>;)
This method must be called before queries are made. The only parameter provided should be an object. All members of this object must be functions that receive only one parameter: a string that uniquely identifies an element in your document tree.

The following example provides all the required handlers to libcss-js. For simplicity, we will use the element id as the identifier string, but you should use something more consistent, such as a hash or a UUID:
```javascript
libcss.init({
  getTagName: function (identifier) {
    // Must return a string with the tag name of the element, such as:
    // 'div', or 'p', or 'body'.
    var element = document.getElementById(identifier);
    return element.tagName;
  },
  getAttributes: function (identifier) {
    // Must return an array of objects in the format:
    // { attribute: <string>, value: <string> }
    // Must include all HTML attributes of the element, such as class, id, etc.
    var element = document.getElementById(identifier);
    var attrs = []
    for (let key in element.attributes) {
      if (element.attributes[key].nodeName) {
        attrs.push({
          attribute: element.attributes[key].nodeName,
          value: element.attributes[key].nodeValue
        })
    }}
    return attrs;
  },
  getSiblings: function (identifier) {
    // Must return an array of identifier strings.
    // This array must contain, in order, all of the parent's children,
    // including the element being queried.
    var element = document.getElementById(identifier);
    var sibs = [];
    for (let s of element.parentElement.children) {
      sibs.push(s.id);
    }
  },
  getAncestors: function (identifier) {
    // Must return an array of identifier strings.
    // This array must contain, in order, the chain of parents of the element.
    // i.e. [ 'parent-id', 'grandparent-id', 'great-grandparent-id' ]
    var parent = document.getElementById(identifier).parentElement;
    var ancs = [];
    while (parent) {
      ancs.push(parent.id);
      parent = parent.parentElement;
    }
    return ancs;
  },
  isEmpty: function (identifier) {
    // Must return boolean.
    var element = document.getElementById(identifier);
    return (element.textContent.trim() !== '' || element.childElementCount > 0);
  },

  // The functions above are needed for libcss-js to navigate your element tree.
  // The ones below are optional:
  isVisited: function (identifier) {
    // Return boolean
  },
  isHover: function (identifier) {
    // Return boolean
  },
  isActive: function (identifier) {
    // Return boolean
  },
  isFocus: function (identifier) {
    // Return boolean
  },
  isEnabled: function (identifier) {
    // Return boolean
  },
  isDisabled: function (identifier) {
    // Return boolean
  },
  isChecked: function (identifier) {
    // Return boolean
  },
  isTarget: function (identifier) {
    // Return boolean
  },
  isLang: function (identifier <string>, language <string>) {
    // Return boolean
    var element = document.getElementById(identifier);
    return (element.lang === language);
  }
});
```
