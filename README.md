# libcss-js
Parses CSS and lets you query the computed style of elements in your document tree.
Uses the [NetSurf browser's LibCSS](http://www.netsurf-browser.org/projects/libcss/), transpiled to JavaScript.

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
var css2 = '#box { color: green; } #parent > .floaty { float: left; }'
libcss.addSheet(css1);
libcss.addSheet(css2);

var parStyle = libcss.getStyle('a-paragraph');
var boxStyle = libcss.getStyle('box');
console.log(parStyle['color']);
// '#0000ff00', formated as #aarrggbb
console.log(boxStyle['width']);
// '70em'

libcss.dropSheets()
parStyle = libcss.getStyle('a-paragraph');
boxStyle = libcss.getStyle('box');
console.log(parStyle['color']);
// '#00000000'
console.log(boxStyle['width']);
// 'auto'

```


## API

### libcss.init(config)
This method enables libcss-js to navigate your document tree. It must be called before queries are made.

**config** _&lt;object>_

All members of this object must be callback functions. The _identifier_ parameter is a string that uniquely identifies an element in your document tree.

The following example provides all the required handlers (callback functions) to libcss-js. For simplicity, we will use the element id as the identifier string, but you should use something more consistent, such as a hash or a UUID:
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
  // The ones below are optional. If not provided, they always return false.
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

### libcss.addSheet(sheet, ?options)
Adds a CSS stylesheet to the selection context.

**sheet** _&lt;string>_
The CSS text to be parsed.

**options** _&lt;object>_
* options.level _&lt;string>_

   CSS level. Accepted values are `'1'`, `'2'`, `'2.1'` and `'3'`. Defaults to `'3'`.


* options.origin _&lt;string>_

   CSS origin. Accepted values are `'author'`, `'user'`, `'ua'`, `'UA'`, `'user agent'`, `'user-agent'` (the last four are equivalent). Defaults to `'author'`.


* options.url _&lt;string>_

   URL from which the CSS file was retrieved. Useful for loading resources with relative paths.


* options.media _&lt;array of strings>_

   The media to apply the CSS rules. Accepted values of the string array are `'tv'`, `'tty'`, `'aural'`, `'print'`, `'screen'`, `'speech'`, `'braille'`, `'embossed'`, `'handheld'`, `'projection'` and `'all'`. Defaults to `'all'`.


* options.allow\_quirks _&lt;boolean>_

   Attempt to parse poorly-formatted CSS. Use only if needed, because it may parse shorthands with undefined behavior. Defaults to `false`.

### libcss.dropSheets()
Deletes all CSS from the selection context.

### libcss.getStyle(identifier, ?options)
Returns an object containing all the computed style for the element that corresponds to the identifier.
The returned object has the format `{ 'property': 'value', ... }`. The available properties are defined upstream, by NetSurf's LibCSS.

**options** _&lt;object>_
* options.pseudo _&lt;string>_

   Pseudo element to be queried. Accepted values are `'none'`, `'first-line'`, `'first-letter'`, `'before'` and `'after'`. Defaults to `'none'`.


* options.media _&lt;string>_

   The medium being queried. Accepted values are `'tv'`, `'tty'`, `'aural'`, `'print'`, `'screen'`, `'speech'`, `'braille'`, `'embossed'`, `'handheld'`, `'projection'` and `'all'`. Defaults to `'all'`.


## Testing
```
npm run test
```
